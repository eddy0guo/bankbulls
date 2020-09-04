#include <bankbulls.hpp>
#include <models.hpp>
#include <log.hpp>
#include <eosio/eosio.hpp>
#include <bankbulls_utils.hpp>
#include <eosio/asset.hpp>
#include <string>
#include<algorithm>


using namespace eosio;
using namespace std;

//todo:action入口参数的合法性以及权限控制
//todo:下注总额小于千分之1.5,bets表中当前局的总额超过1.5%，后续的下注就失败掉
//todo:models里面大量的contract换成在hpp里面的getself（）
//todo:所有入函数,需要判断状态的都要加上
//todo：上庄稀释费
//todo:关于stop时间的控制
//todo:各个表的条目限制
//todo::如果当前局结算没完成就进行下一局，那么log就不在打印，log只输出当前局的
//todo::本轮结算日志,轮次结算要判断当前轮次的情况是流局状态还是进行中
//fixme::轮次流局后的轮次结算看着和正常的一样，不需要做额外的逻辑，
//todo::派彩事件放在startround里面进行
//fixme::流轮次之前，应该先流轮次？
//todo:bets表同时入块导致的created相同的问题
//todo:统一日志等asset还是int类型
//todo::stop bet的时候判断是否需要补妆
//todo::取消的时候，不更新bets玩家的状态直接删掉,下注表结算之后就清掉
//todo::抽牌异常处理：进行5轮后，还没抽齐，则跳出，该局取消，退还所有。
//todo::下庄的金额其实只是摆设
//todo::按照文档流程方案改为延时的，然后逻辑上严格控制（状态时间等），考虑ddos，黑名单攻击等可能的情况
//todo::延时任务最后参数（是否替代的）谨慎使用
//todo::延时交易的到账确认-下版本
//todo::延时720和25秒放在设置项
//todo::再封装一层获取设置值，返回数值类型
// 设置参数

void bankbulls::setstate(string key, string value) {
    name finger_official = get_self();
    require_auth(finger_official);
    bool exist_result = models::global_state::is_valid(name(key));
    check(exist_result, "invalid settings key");

    vector<bankbulls::cur_round> cur_round_list = models::cur_round::list(finger_official);
    int64_t investment_amt = cur_round_list[0].player_investment.amount;
    if ((key == LIMIT_PLAY_BANK_KEY_NAME && investment_amt > 0) ||
        (key == LIMIT_SYSTEM_BANK_KEY_NAME && investment_amt == 0)) {
        int64_t cur_max_bet = strtoll(value.c_str(), NULL, 0);
        string limit_info = game_limit_json(cur_round_list[0].id, cur_max_bet, cur_round_list[0].state);
        models::logs::insert(finger_official, limit_info);
    }

    bool key_exist = models::global_state::is_exist(finger_official, name(key));
    if (key_exist == false) {
        models::global_state::insert(finger_official, name(key), value);
    } else {
        models::global_state::update(finger_official, name(key), value);
    }
    return;
}

//删除设置
void bankbulls::delstate(string key) {
    name finger_official = get_self();
    require_auth(finger_official);
    bool exist_result = models::global_state::is_valid(name(key));
    check(exist_result, "invalid settings key");
    models::global_state::remove(finger_official, name(key));
    return;
}

//下注
//uint32_t bankbulls::bet(uint64_t game_id, name player, asset bet_amount,
//      asset amt_spades, asset amt_hearts, asset amt_clubs, asset amt_diamonds, string sign) {
void bankbulls::bet(name user, name to, asset quantity, string memo) {
    // 结算的时候自己回发起交易给用户，这个时候也会接到通知，要规避掉
    name finger_official = get_self();
    //todo：重点审计
    printf("memo---%s--%s-", memo.data(), to.to_string().data());
    vector<string> params = split_string(memo, "|");


    //name action ,
    uint64_t game_id = stoull(params[1]);
    vector<bankbulls::game> games = models::game::list_by_created(finger_official);
    check(game_id == games[0].id, "The params game id is difference current id" + uint_string(games[0].id));
    check(games[0].state == start_bet_game_state, "this bet is over,please wait next");

    name player = name(params[2]);
    asset bet_amount = string_to_asset(params[3]);
    asset amt_spades = string_to_asset(params[4]);
    asset amt_hearts = string_to_asset(params[5]);
    asset amt_clubs = string_to_asset(params[6]);
    asset amt_diamonds = string_to_asset(params[7]);
    string sign = params[8];
    int64_t sum = amt_spades.amount + amt_hearts.amount + amt_clubs.amount + amt_diamonds.amount;
    /**
    printf("  tp=%s ", to.to_string().data());
    printf("  params0=%s ", params[0].data());
    printf("  quantity.amount=%d ", quantity.amount);
    printf("  bet_amount.amount=%d ", bet_amount.amount);
    printf("  sum=%d ", sum);
    **/

    //投注金额需为底注的3倍，以确保足额赔付，如无需赔付则退回，接口为监听接口
    if (to != finger_official || params[0] != "bet" ||
        quantity.amount != bet_amount.amount * 3 || bet_amount.amount != sum) {
        check(false, "check to,quantity,bankbulls action failed");
    }
    uint32_t created_at = current_time_point().sec_since_epoch();
    uint64_t last_cur_round_id = models::cur_round::get_last_id(finger_official);
    uint64_t last_bets_id = models::bets::get_last_id(finger_official);
    uint64_t one = 1;
    eosio::checksum256 txid = get_current_hash();
    printf("txid=%d", txid);
    //uint64_t txid = uint64_hash(hash);
    //如果脚本玩家进行并发下注，则id可能会有来不及读取的情况，直接反馈插入报错
    models::bets::insert(finger_official, last_bets_id + one, game_id, player, bet_amount, amt_spades,
                         amt_hearts, amt_clubs, amt_diamonds, created_at,
                         txid, pending_settlement_bet_state);

    //更新当前轮次总额
    models::cur_round::update(finger_official, last_cur_round_id, bet_amount,
                              amt_spades, amt_hearts, amt_clubs, amt_diamonds);
    //更新日志
    string json = new_bet_json(game_id, player.to_string(), amt_spades.to_string(), amt_hearts.to_string(),
                               amt_clubs.to_string(), amt_diamonds.to_string(), created_at);
    models::logs::insert(finger_official, json);
    return;
}

// 开局
/*
 * 开局: startbet
todo: 配置项1.停止下注秒数stop_time = 25S
2.未来开牌区块号: cur_block + 27* 2, 1秒增长2个高度
3.随机seed并进行签名sign
GNow = RNow + 6
 * */
void bankbulls::startbet(uint64_t block_index, string sign, string public_key) {
    print("start bet");
    // 需要服务的账户权限
    name finger_official = get_self();
    require_auth(finger_official);
    uint64_t balance = get_account_balance(finger_official);
    //todo::状态大于3不然报错
    //系统余额不足的清理停掉该局游戏
    bankbulls::global_state limit_system_bank = models::global_state::find(finger_official,
                                                                           name(LIMIT_SYSTEM_BANK_KEY_NAME));
    uint64_t limit_system_bank_amount = strtoull(limit_system_bank.value.c_str(), NULL, 0);
    if (balance < 20 * limit_system_bank_amount) {
        check(false, "The system balance is not enough to start the game ");
    }
    // todo: sign合法性检查和block_index
    uint32_t created_at = current_time_point().sec_since_epoch();
    uint32_t stop_at = created_at + 25;
    uint64_t last_game_id = models::game::get_last_id(finger_official);
    vector<bankbulls::cur_round> cur_rounds = models::cur_round::list_by_created(finger_official);
    check(cur_rounds.size() == 1, "curound have been created before start bet");
    uint64_t last_round_id = cur_rounds[0].id;

    checksum256 current_txid = get_current_hash();
    string txid_str = sha256_to_hex(current_txid);
    models::game::insert(finger_official, last_game_id + 1, sign, public_key, start_bet_game_state,
                         0,
                         "", "", "", "",
                         "", 0, 0, 0,
                         0, 0, created_at, stop_at,
                         block_index, "", "", "", txid_str, "");
    models::cur_round::update(finger_official, last_round_id, last_game_id + 1);

    //日志刷新
    vector<bankbulls::logs> logs_arr = models::logs::list(finger_official);
    uint64_t logs_id = logs_arr.size() == 0 ? 1 : logs_arr[0].id + 1;
    models::logs::clear(finger_official);
    //在models模块里logs2id自增设置
    // vector <bankbulls::logs2> logs2_arr = models::logs2::list(finger_official);
    // uint64_t logs2_id = logs2_arr.size() == 0 ? 0 : logs2_arr[0].id;
    // models::global_state::update(finger_official,name("lastlogs2id"),to_string(logs2_id));
    models::logs2::clear(finger_official);


    string game_info_json = game_start_json(last_game_id + 1, start_bet_game_state, stop_at, sign, block_index,
                                            cur_rounds[0].id, cur_rounds[0].stop_at,
                                            cur_rounds[0].player_banker_balance.to_string(),
                                            cur_rounds[0].system_banker_balance.to_string(),
                                            cur_rounds[0].player_banker_max_bet_amount.to_string(),
                                            cur_rounds[0].system_banker_max_bet_amount.to_string(),
                                            cur_rounds[0].banker_num,
                                            cur_rounds[0].cur_max_bet.to_string());
    printf("  *%s*   ", game_info_json.data());
    models::logs::insert(finger_official, game_info_json, logs_id);

    //todo:25
    /**
    uint64_t result_id = cancel_deferred(STOP_BET_SID);
    if (result_id == 0) { printf("%s", "交易没有发现"); };
    if (result_id == 1) { printf("%s", "交易取消成功"); };
     **/
    delay_call_action(finger_official, finger_official, name("stopbet"),
                      std::make_tuple(), 25, STOP_BET_SID);

    return;
}

/*
 * deG30pv6x2z86RQd8tbtAWYiV5nBZUqJbYBvp6kAu81xZGlrqSr5W0RTfEg9SFct
 * SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe
 * EOS67sxYKu379Q9QLb3ERgza8oCN21Z3Rr3FDFii1gzj8iDGifxE7
 *
 * void eosio::assert_recover_key(
const eosio::checksum256 & digest,
const eosio::signature & sig,
const eosio::public_key & pubkey
)
 * */
void bankbulls::testsign(checksum256 &house_seed_hash, signature &sig, public_key &pubkey) {
    printf("start verify");
    assert_recover_key(house_seed_hash, sig, pubkey);
    return;
}

//停止下注
void bankbulls::stopbet() {
    name finger_official = get_self();
    require_auth(finger_official);
    vector<bankbulls::game> games = models::game::list_by_created(finger_official);
    //last_game_id必须是下注中
    check(games[0].state == start_bet_game_state,"can stop game behind start start");
    models::game::update(finger_official, games[0].id, stop_bet_game_state);
    bankbulls::game last_game = models::game::find(finger_official, games[0].id);

    string json = stop_bet_json(games[0].id, stop_bet_game_state);
    models::logs::insert(finger_official, json);
    return;
}

// 开牌接口
/**
    * 1、校验权限
    * 2、校验sign和seed和SERVICE_PUBKEY是否对上
    * 3、更新相关业务表
    * 4、
    */
//void bankbulls::testsign(checksum256 &house_seed_hash, signature &sig, public_key &pubkey) {
//uint32_t bankbulls::opencards(string sign_str, checksum256 seed,uint64_t block_index, string block_hash) {
//fixme：hex_to_sha256方法暂时不可用，这里的seed先用系统的强转，对于memo来说不变的都是传字符串
void bankbulls::opencards(string sign_str, checksum256 seed, uint64_t block_index, string block_hash) {
    //todo:检查时间是否小于stopat，状态是否改为停止下注
    //todo:block_index和block_hash的对应关系
    name finger_official = get_self();
    require_auth(finger_official);
    string seed_str = sha256_to_hex(seed);

    vector<bankbulls::game> game_list = models::game::list_by_created(finger_official);
    check(game_list[0].state == stop_bet_game_state,"opencards must behind stop bet2");
    const public_key pub_key = str_to_pub(game_list[0].public_key);
    const signature sign = str_to_sig(sign_str);
    eosio::assert_recover_key(seed, sign, pub_key);

    //fixme:更新下注表所有玩家的状态?还是结算开始的时候结算
    uint64_t last_game_id = game_list[0].id;
    bankbulls::bets_index bets_table(finger_official, finger_official.value);
    auto game_id_index = bets_table.get_index<"gameid"_n>();
    auto itr = game_id_index.find(last_game_id);
    while (itr != game_id_index.end()) {
        game_id_index.modify(itr, finger_official, [&](auto &p) {
            p.state = start_settlement_bet_state;
        });
        itr++;
    }
    //更新game表状态
    uint32_t reveal_at = current_time_point().sec_since_epoch();
    //todo:信息应该在最后更新
    models::game::update(finger_official, last_game_id,open_card_game_state, reveal_at, seed_str, block_hash);
    //洗牌
    // printf("seed=%s--", sha256_to_hex(seed).data());
    CARDS_ALLOCATION instance[5] = {};
    //CARDS_ALLOCATION mInstance = CARDS_ALLOCATION_DEFAULT;
    string wash_result = wash_hash(sha256_to_hex(seed), block_hash);
    //printf("wash hash =%s--", wash_result.data());
    //抽取25张牌，p1,p2,p3,p4,p5每组五张牌
    CARDS_ALLOCATIONS cards_allocations = deal_cards(wash_result);
    // printf("   kkkk=%s--    ", cards_allocations.to_json().data());
    //计算每组手牌的最大的牌型
    get_max_combination(cards_allocations);
    // printf("   mmm=%s--   ", cards_allocations.to_json().data());
    //计算庄和手牌顺序,排序后，下标0为庄，其他依次为黑红梅方的闲家
    char sort_char = sort_allocations(cards_allocations, block_hash);
    // printf("   nnn=%s--   ", cards_allocations.to_json().data());
    //todo::庄的牌型和闲家牌型比输赢,然后更新到game表和区块中
    vector<string> allocations_log_info = flush_cards_allocations_to_table(finger_official, cards_allocations,
                                                                           wash_result, block_hash, sort_char);
    flush_cards_allocations_to_block(finger_official);
    for (const auto &item : allocations_log_info) {
        printf("   %s   ", item.data());
    }
    string game_info = game_result_json(last_game_id, open_card_game_state, seed_str, wash_result,
                                        block_index, block_hash, allocations_log_info[0],
                                        allocations_log_info[1], allocations_log_info[2],
                                        allocations_log_info[3], allocations_log_info[4]);
    models::logs::insert(finger_official, game_info);

    delay_call_action(finger_official, finger_official, name("sltgame"),
                      std::make_tuple(20, last_game_id), 1, SLT_GAME_SID);
    return;
}

//结算接口
// settlement 缩写为slt
void bankbulls::sltgame(uint32_t dispose_count, int32_t game_id) {
    name finger_official = get_self();
    require_auth(finger_official);
    //如果系统没坐庄，若某局投注总额＞玩家庄池*1.5%，为不影响大额玩家体验，系统将参与补庄，并与玩家庄按份额共担盈亏。
    //待测试
    check(dispose_count <= 20, "dispose_count must be less than 20 eos");
    int64_t sum_bets_amt = 0;
    vector<bankbulls::cur_round> cur_rounds = models::cur_round::list_by_created(finger_official);
    check(cur_rounds.size() > 0, "cur_rounds size can be less than zero");
    vector<bankbulls::bets> bets_list = models::bets::list(finger_official, game_id,start_settlement_bet_state);
    for (auto item:bets_list) {
        sum_bets_amt += item.bet_amount.amount;
    }
    int sys_amt = cur_rounds[0].system_investment.amount;
    int ply_amt = cur_rounds[0].player_investment.amount;
    int bets_amt_limit = ply_amt * 15 / 1000;
    if (sys_amt == 0 && sum_bets_amt > bets_amt_limit) {
        system_join_bankers(finger_official);
    }
    start_settlement_game(finger_official, dispose_count, game_id);
    vector<bankbulls::bets> bets_list2 = models::bets::list(finger_official, game_id, start_settlement_bet_state);
    if (bets_list2.size() != 0) {
        //void delay_call_action(name contract,name fee_account,name action,T tuple_info,uint32_t delay_time,uint64_t sender_id){
        delay_call_action(finger_official, finger_official, name("sltgame"),
                          std::make_tuple(dispose_count, game_id), 1, SLT_GAME_SID);
    }

    return;
}

//8.7.1 轮次结算
// settlement 缩写为slt
//todo::dispose_count 用起来
void bankbulls::sltround(int32_t dispose_count, uint32_t count) {
    name finger_official = get_self();
    require_auth(finger_official);
    bankbulls::global_state min_down_bank = models::global_state::find(finger_official, name(MIN_DOWN_BANK_KEY_NAME));
    uint64_t min_down_bank_amount = strtoull(min_down_bank.value.c_str(), NULL, 0);
    asset min_down_bank_asset = asset(min_down_bank_amount, EOS_SYMBOL);
    printf("start sltround");
    vector<bankbulls::up_bankers> up_bankers_list =
            models::up_bankers::list(finger_official, res_down_banker_state, res_reduce_banker_state);
    if (up_bankers_list.size() == 0) {
        printf("have no banker need to be settlement");
        return;
    }
    /**
    check(dispose_count <= up_bankers_list.size(),
          "dispose_count must be less than up_bankers_list size which state is not up_banker_state");
          **/
    dispose_count = dispose_count <= up_bankers_list.size() ? dispose_count : up_bankers_list.size();
    for (uint32_t j = 0; j != dispose_count; j++) {
        //结算完毕更新状态为up_banker_state,todo::ssss
        models::up_bankers::update(finger_official, up_bankers_list[j].account, up_banker_state, "ssss");
        int64_t payout_all = up_bankers_list[j].res_amt.amount;
        if (up_bankers_list[j].state == res_down_banker_state) {
            string memo = "banker bulls down bank settlement";
            delay_call_action(name("eosio.token"), finger_official, name("transfer"),
                              std::make_tuple(finger_official, up_bankers_list[j].account, up_bankers_list[j].res_amt,
                                              std::string(memo)),
                              1, SLT_ROUND_SID + j + 1);
            models::up_bankers::remove(finger_official, up_bankers_list[j].account);
        } else if (up_bankers_list[j].state == res_reduce_banker_state) {
            string memo = "banker bulls reduce bank settlement";
            //投资额度小于最小上庄总额(10 eos)强制下庄,
            //todo:待制造场景测试
            if (up_bankers_list[j].investment.amount < min_down_bank_amount) {
                string memo = "investment balance is" + up_bankers_list[j].investment.to_string() +
                              ";it's less than min_down limit " + min_down_bank_asset.to_string() +
                              "and force down bank this account";
                payout_all = up_bankers_list[j].investment.amount;
                models::up_bankers::remove(finger_official, up_bankers_list[j].account);

            }
            asset reduce_asset = asset(payout_all, EOS_SYMBOL);
            delay_call_action(name("eosio.token"),
                              finger_official,
                              name("transfer"),
                              std::make_tuple(finger_official, up_bankers_list[j].account, reduce_asset,
                                              std::string(memo)),
                              1, SLT_ROUND_SID + j + 1);
        } else {
            printf("Neither reduce_banker_action nor down_bank_action");
            continue;
        }
    }
    if (up_bankers_list.size() == dispose_count) {
        //轮次结算完成日志
        uint64_t cur_round_id = up_bankers_list[0].start_round_id;
        string slt_round_info = round_slt_finish_json(cur_round_id, finish_settlement_round_state);
        models::logs2::insert(finger_official, slt_round_info);
        printf("settlement finished");
    } else {
        //轮次结算进行中
        printf("settlementing...");
        delay_call_action(finger_official, finger_official, name("sltround"), std::make_tuple(20, 1), 1, SLT_ROUND_SID);
    }
    return;
}

//8.7.2 轮次开始
/**
 * todo：1,查询系统余额
        2,监听eosio.token 余额实时更新
        3,12分钟为配置项
        4,三个最大下注额度为配置项
 */
void bankbulls::startround() {
    print("startround ");
    name finger_official = get_self();
    require_auth(finger_official);
    //todo:冗余判断下所以的upbanker状态必须为1，
    vector<bankbulls::up_bankers> up_bankers_list = models::up_bankers::list(finger_official);
    vector<bankbulls::cur_round> cur_round_list = models::cur_round::list(finger_official);
    uint64_t last_round_id;

    //创世轮次
    if (up_bankers_list.size() == 0 && cur_round_list.size() == 0) {
        last_round_id = 0;
        //后续轮次
    } else if (up_bankers_list.size() > 0 && cur_round_list.size() > 0) {
        //非创世轮次，每轮开始之前判断当前局是否已经结算完成，没完成的话就回掉等到完成再开始新的轮次
        vector<bankbulls::game> games = models::game::list_by_created(finger_official);
        if(games[0].state != finish_settlement_game_state && games[0].state != cancel_settlement_game_state){
            delay_call_action(finger_official, finger_official, name("startround"), std::make_tuple(), 1, START_ROUND_SID);
            return;
        }

        check(cur_round_list.size() == 1, "internal error cur_round_list size must be 1");
        check(up_bankers_list.size() > 0, "internal error, up bankers table have nothing");
        int64_t sum_total_divide = 0;
        last_round_id = cur_round_list[0].id;
        //根据当前轮次表的总输赢和upbanker表的庄占比更新upbaner的分红更新
        for (uint32_t i = 0; i < up_bankers_list.size(); i++) {
            int64_t total_divide_amount = 0;
            int64_t total_divide_amount_nano = cur_round_list[0].banker_win_lose.amount * up_bankers_list[i].bet_ratio;
            //前面的玩家盈亏向下取整，最后一位兜底剩下的额度
            if (i == up_bankers_list.size() - 1) {
                total_divide_amount = cur_round_list[0].banker_win_lose.amount - sum_total_divide;
            } else {
                total_divide_amount = total_divide_amount_nano / 1000000;
                sum_total_divide += total_divide_amount;
            }
            //庄家赢扣手续费,由于这个补偿，平台稍微有点让利0.0001eos，输了平摊损失
            total_divide_amount = total_divide_amount < 0 ? total_divide_amount : total_divide_amount * 985 / 1000 + 1;
            int64_t next_investment_amount = up_bankers_list[i].investment.amount + total_divide_amount;
            models::up_bankers::update(finger_official, up_bankers_list[i].account, total_divide_amount,next_investment_amount);
        }
    } else {
        check(false, "Unknown circumstances when start  round");
    }
    update_up_banker_and_settlement(finger_official);
    //todo:轮次开始读取预约表中数据更新到当前轮次表里的player_banker_max_bet_amount，
    models::res_bankers::clear(finger_official);

    uint64_t last_curround_id = 0;
    if (cur_round_list.size() > 0) {
        insert_hisround_from_curound(finger_official, cur_round_list[0]);
        last_curround_id = cur_round_list[0].id;

        //派彩日志
        int bwl_amt = cur_round_list[0].banker_win_lose.amount;
        int64_t banker_payout_total = bwl_amt <= 0 ? bwl_amt : bwl_amt * 985 / 1000;
        asset banker_payout_total_asset = asset(banker_payout_total, EOS_SYMBOL);
        string round_slt_info = round_slt_payout_json(last_curround_id,
                                                      banker_payout_total_asset, start_settlement_round_state);
        models::logs::insert(finger_official, round_slt_info);
    }
    models::cur_round::clear(finger_official);
    update_cur_round(finger_official, last_curround_id);

    //轮次开始日志
    string round_start_info = round_begin_json(last_curround_id + 1, ongoing_round_state);
    models::logs::insert(finger_official, round_start_info);
    delay_call_action(finger_official, finger_official, name("startround"), std::make_tuple(), 120, START_ROUND_SID);
    //1没用
    delay_call_action(finger_official, finger_official, name("sltround"), std::make_tuple(20, 1), 1, SLT_ROUND_SID);
    return;
}

//8.8预约接口
//action:预约动作:上庄：1,下庄：2,上调：3,下调：4
//预约动作：
//上庄：1
//下庄：2,此时的金额可有可无
//上调：3
//下调：4
//取消上庄：5，取消为预约上庄阶段的取消
void bankbulls::resbank(name account, asset investment, uint64_t action) {
    require_auth(account);
    //判断入口action是监听的eosio.token的还是官方的
    eosio::action act = eosio::get_action(1, 0);
    printf(" action_name=%s ", act.name.to_string().data());
    if (act.name != name("transfer")) {
        if (action == up_banker_action || action == increase_banker_action) {
            check(false, "resbank action cann't be up_banker_action or increase_banker_action");
        }
    }

    name finger_official = get_self();
    uint32_t created_at = current_time_point().sec_since_epoch();
    bankbulls::global_state min_up_amount = models::global_state::find(finger_official, name(MIN_UP_BANK_KEY_NAME));
    int64_t min_up_amount_value = strtoull(min_up_amount.value.c_str(), NULL, 0);
    asset min_up_asset = asset(min_up_amount_value, EOS_SYMBOL);


    bankbulls::global_state min_increase_amount = models::global_state::find(finger_official,
                                                                             name(MIN_INCREASE_KEY_NAME));
    int64_t min_increase_amount_value = strtoull(min_increase_amount.value.c_str(), NULL, 0);
    asset min_increase_asset = asset(min_increase_amount_value, EOS_SYMBOL);

    bankbulls::global_state min_reduce_amount = models::global_state::find(finger_official, name(MIN_REDUCE_KEY_NAME));
    int64_t min_reduce_amount_value = strtoull(min_reduce_amount.value.c_str(), NULL, 0);
    asset min_reduce_asset = asset(min_reduce_amount_value, EOS_SYMBOL);


    // todo:下调和上调的金额检查
    switch (action) {
        case up_banker_action: {
            printf("up bankers");
            check(investment.amount >= min_up_amount_value,
                  "up_banker_ amount must be more than " + min_up_asset.to_string());
            models::res_bankers::insert(finger_official, account, investment, action, created_at);
            break;
        }
        case down_bank_action:
        case increase_banker_action: {
            check(investment.amount >= min_increase_amount_value,
                  "up_banker_ amount must be more than " + min_increase_asset.to_string());
            bool is_exist_result = models::up_bankers::is_exist(finger_official, account);
            check(is_exist_result, "account is not a up_bankers");
            models::res_bankers::insert(finger_official, account, investment, action, created_at);
            break;
        }

        case reduce_banker_action: {
            bankbulls::global_state limit_system_bank = models::global_state::find(finger_official,
                                                                                   name(LIMIT_SYSTEM_BANK_KEY_NAME));
            uint64_t limit_system_bank_amount = strtoull(limit_system_bank.value.c_str(), NULL, 0);

            bankbulls::up_bankers banker = models::up_bankers::find(finger_official, account);
            int64_t remained_investment =
                    banker.investment.amount + banker.total_divide.amount - limit_system_bank_amount;
            asset remained_investment_asset = asset(remained_investment, EOS_SYMBOL);

            if (investment.amount < min_reduce_amount_value) {
                check(false, "reduce_banker_ amount must be more than " + min_reduce_asset.to_string());
            } else if (investment.amount > remained_investment) {
                check(false, "reduce_banker_ amount must be less than " + remained_investment_asset.to_string());
            } else {}
            bool is_exist_result = models::up_bankers::is_exist(finger_official, account);
            check(is_exist_result, "account is not a up_bankers");
            models::res_bankers::insert(finger_official, account, investment, action, created_at);
            break;
        }
        case cancel_banker_action: {
            printf("cancel bankers");
            bool insert_result = models::res_bankers::remove(finger_official, account);
            check(insert_result, "remove res_bankers table failed");
            break;
        }
        default:
            check(false, "unknown action");
            break;
    }
    return;
}


/**
 * 1.针对游戏
发送cancelgame流局接口，更新games表即可
2.针对轮次
发送cancelround流轮次接口，更新当前轮次表即可，
 * **/
void bankbulls::cancelgame() {
    name finger_official = get_self();
    require_auth(finger_official);
    //uint64_t last_id = models::game::get_last_id(finger_official);
    vector<bankbulls::game> games = models::game::list_by_created(finger_official);
    //todo:待join确认
    //fixme:只有state小于3的才流局，其他的直接走正常流程
    check(games[0].state <= 2, "game have opend card,can't cancel");
    if (games[0].state == start_bet_game_state) {
        cancel_deferred(STOP_BET_SID);
    }
    uint64_t last_id = games[0].id;
    models::game::update(finger_official, last_id, cancel_settlement_game_state);
    string cancel_game_info = game_cancel_json(last_id, cancel_settlement_game_state);
    models::logs::insert(finger_official, cancel_game_info);

    bankbulls::bets_index bets_table(finger_official, finger_official.value);
    auto game_id_index = bets_table.get_index<"gameid"_n>();
    auto itr = game_id_index.find(last_id);
    while (itr != game_id_index.end()) {
        game_id_index.modify(itr, finger_official, [&](auto &p) {
            p.state = start_settlement_bet_state;
        });
        itr++;
    }
    //
    delay_call_action(finger_official, finger_official, name("sltgame"), std::make_tuple(20, last_id), 1, SLT_GAME_SID);
}

void bankbulls::cancelround() {
    name finger_official = get_self();
    require_auth(finger_official);
    vector<bankbulls::cur_round> cur_rounds = models::cur_round::list_by_created(finger_official);
    uint64_t last_id = cur_rounds[0].id;
    check(cur_rounds[0].state == ongoing_round_state, "this round is settlementing round,can't cancel");
    //流轮之前先流局
    bankbulls::cancelgame();
    //uint64_t last_id = models::cur_round::get_last_id(finger_official);
    models::cur_round::update(finger_official, cancel_settlement_round_state);
    string cancel_round_info = round_cancel_json(last_id, cancel_settlement_round_state);
    models::logs::insert(finger_official, cancel_round_info);
    cancel_deferred(START_ROUND_SID);
    delay_call_action(finger_official, finger_official, name("startround"), std::make_tuple(), 1, START_ROUND_SID);
}

void bankbulls::testdelayc() {
    eosio::transaction txn{};
    string ss = "max_time";
    uint32_t bb = 3322;
    name contract = get_self();
    uint64_t yi = 1;
    for (int64_t i = 0; i < 20; ++i) {
        {
            txn.actions.emplace_back(
                    eosio::permission_level(_self, "active"_n),
                    contract,
                    "testdelayb"_n, //action名称
                    std::make_tuple(i)
            );//tuplex
            txn.delay_sec = i;
            txn.send(i, _self, false);
        }
        bankbulls::global_state last_logs2_id = models::global_state::find(contract, name(MIN_UP_BANK_KEY_NAME));
        uint64_t id = strtoull(last_logs2_id.value.c_str(), NULL, 0) + 1;
        models::global_state::update(contract, name(MIN_UP_BANK_KEY_NAME), to_string(id));
    }
    return;
}

void bankbulls::testdelayb(int64_t count) {
    eosio::transaction txn{};
    string ss = "max_time";
    uint32_t bb = 3322;
    name contract = get_self();
    uint64_t yi = 1;
    /**
    int64_t postion = count * 15;
    bankbulls::global_state last_logs2_id = models::global_state::find(contract, name(LAST_LOGS2_ID_KEY_NAME));
    uint64_t id = strtoull(last_logs2_id.value.c_str(), NULL, 0) + postion;
     **/
    for (int i = 0; i < 15; ++i) {
        txn.actions.emplace_back(
                eosio::permission_level(_self, "active"_n),
                name("eosio.token"),
                "transfer"_n, //action名称
                std::make_tuple(contract, name("eddytest1115"), asset(yi, EOS_SYMBOL), std::string(""))
        );//tuplex
        txn.delay_sec = 10;
        txn.send(i + count * 51, _self, false);
        //id++;
        bankbulls::global_state last_logs2_id = models::global_state::find(contract, name(LAST_LOGS2_ID_KEY_NAME));
        uint64_t id = strtoull(last_logs2_id.value.c_str(), NULL, 0) + 1;
        models::global_state::update(contract, name(LAST_LOGS2_ID_KEY_NAME), to_string(id));
    }
    return;
}

void bankbulls::testdelay() {
    eosio::transaction txn{};
    string ss = "max_time";
    uint32_t bb = 3322;
    name contract = get_self();
    bankbulls::global_state last_logs2_id = models::global_state::find(contract, name(LAST_LOGS2_ID_KEY_NAME));
    uint64_t id = strtoull(last_logs2_id.value.c_str(), NULL, 0) + 1;
    models::global_state::update(contract, name(LAST_LOGS2_ID_KEY_NAME), to_string(id));
    if (id > 50) {
        return;
    }
    uint32_t tmp1 = 1000000;
    for (int i = 0; i < 20; ++i) {
        eosio::transaction txn2{};
        txn2.actions.emplace_back(
                eosio::permission_level(_self, "active"_n),
                name("eosio.token"),
                "transfer"_n, //action名称
                std::make_tuple(contract, name("eddytest1115"), asset(1, EOS_SYMBOL), std::string(""))
        );
        txn2.delay_sec = 1;
        txn2.send(i + 1, _self, false);
    }
    txn.actions.emplace_back(
            eosio::permission_level(_self, "active"_n),
            _self,
            "testdelay"_n, //action名称
            std::make_tuple());//tuplex
    txn.delay_sec = 1;
    txn.send(999999, _self, false);
    return;
}

void bankbulls::testdelaya() {
    printf("testdelayaaaa");
    string ss = "max_time";
    uint32_t bb = 33;
    for (int i = 0; i < 100; ++i) {
        action(permission_level{_self, name("active")},
               _self,
               name("getdelay"),
               std::make_tuple(ss, i))
                .send();
    }
    return;
}

void bankbulls::getdelay(string key, uint32_t value) {
    //>> a-2 b-834666496 c--1701167104 d-1493172224 e-536870912 f-1459617792 g-1761607680 g-1073741824
    printf("a-%d ", value);
    printf("b-%d ", name("bankbulls").value);  //834666496
    printf("c-%d ", name("startround").value); //1701167104
    printf("d-%d ", name("startbet").value);  //1493172224
    printf("e-%d ", name("stopbet").value);    // 536870912
    printf("f-%d ", name("transfer").value);//  1459617792
    printf("f1-%d ", name("eosio.token").value);//
    printf("g-%d ", name("sltround").value);  //1761607680
    printf("h-%d ", name("sltgame").value);   //1073741824


    //name finger_official = get_self();
    // require_auth(finger_official);
    //check(table_key != STATE_KEY.end(), "unknown key");
    //uint32_t value = 12567;
    // bool result = models::global_state::insert(finger_official, table_key->second, value);
    eosio::transaction txn2{};
    txn2.actions.emplace_back(
            eosio::permission_level(_self, "active"_n),
            name("eosio.token"),
            "transfer"_n, //action名称
            std::make_tuple(get_self(), name("eddytest1115"), asset(value, EOS_SYMBOL), std::string(""))
    );
    txn2.delay_sec = 60;
    txn2.send(1, _self, false);

    return;
}

void bankbulls::tableclear(string table) {
    //name finger_official = get_self();
    //require_auth(finger_official);
    //eosio.token transfer id不能确定，不取消
    cancel_deferred(SLT_ROUND_SID);
    cancel_deferred(SLT_GAME_SID);
    cancel_deferred(START_ROUND_SID);
    cancel_deferred(STOP_BET_SID);

    name finger_official = get_self();
    require_auth(finger_official);
    if (table == "resbankers") {
        models::res_bankers::clear(finger_official);
    } else if (table == "games") {
        models::game::clear(finger_official);
    } else if (table == "globalstate") {
        models::global_state::clear(finger_official);
    } else if (table == "bets1") {
        models::bets::clear(finger_official);
    } else if (table == "upbankers") {
        models::up_bankers::clear(finger_official);
    } else if (table == "hisrounds") {
        models::his_round::clear(finger_official);
    } else if (table == "curround") {
        models::cur_round::clear(finger_official);
    } else if (table == "logs") {
        models::logs::clear(finger_official);
    } else if (table == "logs2") {
        models::logs2::clear(finger_official);
    } else if (table == "all") {
        models::res_bankers::clear(finger_official);
        models::game::clear(finger_official);
        models::global_state::clear(finger_official);
        models::bets::clear(finger_official);
        models::up_bankers::clear(finger_official);
        models::his_round::clear(finger_official);
        models::cur_round::clear(finger_official);
        models::logs2::clear(finger_official);
        models::logs::clear(finger_official);
    } else {
        check(false, "unknown params");
    }
    return;
}

void bankbulls::delayclear() {
    cancel_deferred(SLT_ROUND_SID);
    cancel_deferred(SLT_GAME_SID);
    cancel_deferred(START_ROUND_SID);
    cancel_deferred(STOP_BET_SID);
}


void bankbulls::canceltrx(name player) {
    uint64_t result_id = cancel_deferred(1);
    if (result_id == 0) { printf("%s", "交易没有发现"); };
    if (result_id == 1) { printf("%s", "交易取消成功"); };
}


