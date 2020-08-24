#include <bankbulls.hpp>
#include <models.hpp>
#include <log.hpp>
#include <eosio/eosio.hpp>
#include <bankbulls_utils.hpp>
#include <string>
#include<algorithm>


using namespace eosio;
using namespace std;

//#include <type.hpp>
// const map<string,int> m3 = { {"string",1}, {"sec",2}, {"trd",3} };
//todo:action入口参数的合法性以及权限控制
//todo:系统余额不足的清理停掉该局游戏
//todo:下注总额小于千分之1.5,bets表中当前局的总额超过1.5%，后续的下注就失败掉
//todo:memo分隔符
//todo:下注更新当前轮次的下注额度
//todo：下注的时候判断需不需要系统补妆
//todo：判断轮次开始的时候需不需要补妆
//todo：每局结算的时候，更新相关赔付金额和余额等
//todo: 日志表
//todo:models里面大量的contract换成在hpp里面的getself（）
//todo:当前轮次的输赢额度是存绝对值还是带正负
//todo:所有入函数,需要判断状态的都要加上
//todo:日志逻辑
//todo：设置项动态获取
//todo:系统中途补庄逻辑
//todo:总额不足，用户下注量等各种阀值判断
//todo：上庄稀释费
//todo::测试下庄，增加，减少的逻辑
//todo:简单的错误吗设计
//todo:关于stop时间的控制
//todo:各个表的条目限制
//todo:轮次结算的时候扣除手续费
void play(name finger_official, name user, uint32_t asset_amount, string token_contract_name, string asset_name,
          string memo) {

    bankbulls::records records_table(name(finger_official), name(finger_official).value);
    if (memo != "rock" && memo != "paper" && memo != "scissors") {
        // 调用错误的话，不赔偿
        return;
    }
    require_auth(user);
    string user_gestures = memo;
    // rock  paper scissors
    uint32_t now = current_time_point().sec_since_epoch();
    // print("sender is ",current_receiver());
    string eos_gestures = get_eos_gestures();
    int result = computer_game_result(user_gestures, eos_gestures);
    if (result == 1) {
        // 只做一倍的赌注
        finger_transfer(finger_official, user, token_contract_name, asset_amount * 2, "you win!!!");
    } else if (result == 0) {
        finger_transfer(finger_official, user, token_contract_name, asset_amount, "game tied");
    } else {
        print("you lose!");
    }

    auto time_idx = records_table.get_index<"time"_n>();
    auto last_meet_itr = time_idx.begin();
    uint32_t id = last_meet_itr == time_idx.end() ? 1 : last_meet_itr->id + 1;

    records_table.emplace(finger_official, [&](auto &f) {
        f.id = id;
        print("current records id is ", f.id);
        f.player = user;
        f.user_gestures = user_gestures;
        f.eos_gestures = eos_gestures;
        f.asset_name = asset_name;
        f.asset_amount = asset_amount;
        f.result = result;
        f.playTime = now;
    });
}


void bankbulls::playeos(name user, name to, asset quantity, string memo) {
    if (quantity.symbol != EOS_SYMBOL) {
        return;
    }
    name finger_official = get_self();
    play(finger_official, user, quantity.amount, "eosio.token", "EOS", memo);
}

void bankbulls::play_finger(name user, name to, asset quantity, string memo) {
    name finger_official = get_self();
    if (quantity.symbol != FINGER_SYMBOL || to != finger_official) {
        return;
    }
    play(finger_official, user, quantity.amount, "eddytest1113", "FINGER", memo);
}

// 设置参数
bool bankbulls::setstate(string key, uint32_t value) {
    name finger_official = get_self();
    // require_auth(finger_official);
    auto table_key = STATE_KEY.find(key);
    check(table_key != STATE_KEY.end(), "unknown key");
    printf("table_key is %d", table_key);
    bool result = models::global_state::insert(finger_official, table_key->second, value);
    check(result, "insert global_state table failed");
    return true;
}

//删除设置
bool bankbulls::delstate(string key) {
    name finger_official = get_self();
    // 理论上只能合约部署账户才能调用此接口，这里为了方便调试先注释
    // require_auth(finger_official);
    auto table_key = STATE_KEY.find(key);
    check(table_key != STATE_KEY.end(), "unknown key");
    printf("table_key is %d", table_key);
    bool result = models::global_state::remove(finger_official, table_key->second);
    check(result, "insert global_state table failed");
    return true;
}

//下注
//uint32_t bankbulls::bet(uint64_t game_id, name player, asset bet_amount,
//      asset amt_spades, asset amt_hearts, asset amt_clubs, asset amt_diamonds, string sign) {
uint32_t bankbulls::bet(name user, name to, asset quantity, string memo) {
    // 结算的时候自己回发起交易给用户，这个时候也会接到通知，要规避掉
    if (user == get_self()) {
        return 0;
    }
    //todo：重点审计
    printf("memo---%s--%s-", memo.data(), to.to_string().data());
    vector <string> params = split_string(memo, ",");


    //name action ,
    uint64_t game_id = stoull(params[1]);
    name player = name(params[2]);
    asset bet_amount = string_to_asset(params[3]);
    asset amt_spades = string_to_asset(params[4]);
    asset amt_hearts = string_to_asset(params[5]);
    asset amt_clubs = string_to_asset(params[6]);
    asset amt_diamonds = string_to_asset(params[7]);
    string sign = params[8];
    int64_t sum = amt_spades.amount + amt_hearts.amount + amt_clubs.amount + amt_diamonds.amount;
    ////todo:1.投注金额需为底注的3倍，以确保足额赔付，如无需赔付则退回，接口为监听接口
    printf("  tp=%s ", to.to_string().data());
    printf("  params0=%s ", params[0].data());
    printf("  quantity.amount=%d ", quantity.amount);
    printf("  bet_amount.amount=%d ", bet_amount.amount);
    printf("  sum=%d ", sum);


    if (to != _self || params[0] != "bet" ||
        quantity.amount != bet_amount.amount * 3 || bet_amount.amount != sum) {
        check(false, "check to,quantity,bankbulls_action failed");
    }
    // require_auth(player);
    //todo:检查gameid，和当前id是否一致
    name finger_official = get_self();
    uint32_t created_at = current_time_point().sec_since_epoch();
    uint64_t last_cur_round_id = models::cur_round::get_last_id(finger_official);
    uint64_t last_bets_id = models::bets::get_last_id(finger_official);
    uint64_t one = 1;
    eosio::checksum256 txid = get_current_hash();
    printf("txid=%d", txid);
    //uint64_t txid = uint64_hash(hash);
    //fixme：如果脚本玩家进行并发下注，则id可能会有来不及读取的情况，直接反馈插入报错
    //fixme：用外部传人的gameid和用查到的当前last_game_id?
    //fixme:检查总额和红黑梅方四个方向的额度之和是否相等
    printf("%llu--%llu---", last_bets_id, last_bets_id + one);
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
    return 0;
}

// 开局
/*
 * 开局: startbet
todo: 配置项1.停止下注秒数stop_time = 25S
2.未来开牌区块号: cur_block + 27* 2, 1秒增长2个高度
3.随机seed并进行签名sign
GNow = RNow + 6
 * */
uint32_t bankbulls::startbet(uint64_t block_index, string sign) {
    print("start bet");
    // 需要服务的账户权限
    name finger_official = get_self();
    require_auth(finger_official);
    // todo: sign合法性检查和block_index
    uint32_t created_at = current_time_point().sec_since_epoch();
    uint32_t stop_at = created_at + 25;
    uint64_t last_game_id = models::game::get_last_id(finger_official);
    vector <models::bankbulls::cur_round> cur_rounds = models::cur_round::list_by_created(finger_official);
    check(cur_rounds.size() == 1, "curound have been created before start bet");
    // uint64_t last_round_id = models::cur_round::get_last_id(finger_official);
    uint64_t last_round_id = cur_rounds[0].id;

    /*
     *  static bool insert(name contract,uint64_t id,string sign,uint32_t state,uint32_t dealer_hand_number,
                           string dealer_card,string spades_card,string hearts_card,string clubs_card,
                           string diamonds_card,int32_t spades_result,int32_t hearts_result,int32_t clubs_result,
                           int32_t diamonds_result,uint32_t reveal_at,uint64_t created_at,uint64_t stop_at,
                           uint64_t block_index,string block_hash,string seed,string wash_hash)
     *
     * */
    // fixme:初始默认值是否合理？
    bool insert_result = models::game::insert(finger_official, last_game_id + 1, sign, start_bet_game_state, 0,
                                              "", "", "", "",
                                              "", 0, 0, 0,
                                              0, 0, created_at, stop_at,
                                              block_index, "", "", "");
    //todo 更新轮次的当前id
    bool update_cur_round_result = models::cur_round::update(finger_official, last_round_id, last_game_id + 1);

    //日志刷新
    models::logs::clear(finger_official);
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
    models::logs::insert(finger_official, game_info_json);

    return 0;
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
}

//停止下注
uint32_t bankbulls::stopbet() {
    //更新game表状态
    printf("stop bet");
    name finger_official = get_self();
    require_auth(finger_official);
    uint64_t last_game_id = models::game::get_last_id(finger_official);
    // last_game_id必须是下注中
    models::game::update(finger_official, last_game_id, stop_bet_game_state);
    models::bankbulls::game last_game = models::game::find(finger_official, last_game_id);
    printf("-----%d--", last_game.id);

    string json = stop_bet_json(last_game_id, stop_bet_game_state);
    models::logs::insert(finger_official, json);
    return 0;
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
uint32_t bankbulls::opencards(string sign_str, checksum256 seed, uint64_t block_index, string block_hash) {
    //todo:检查时间是否小于stopat，状态是否改为停止下注
    //todo:block_index和block_hash的对应关系
    name finger_official = get_self();
    require_auth(finger_official);
    string seed_str = sha256_to_hex(seed);
    printf("start open cards,service pub key is %s", SERVICE_PUBKEY.data());
    const public_key pub_key = str_to_pub(SERVICE_PUBKEY);
    const signature sign = str_to_sig(sign_str);
    eosio::assert_recover_key(seed, sign, pub_key);
    //fixme:更新下注表所有玩家的状态?还是结算开始的时候结算
    uint64_t last_game_id = models::game::get_last_id(finger_official);
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
    bool update_result = models::game::update(finger_official, last_game_id,
                                              open_card_game_state, reveal_at, seed_str, block_hash);
    check(update_result, "update failed");

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
    vector <string> allocations_log_info = flush_cards_allocations_to_table(finger_official, cards_allocations,
                                                                            wash_result, block_hash, sort_char);
    flush_cards_allocations_to_block(finger_official);
    for(const auto & item : allocations_log_info){
        printf("   %s   ",item.data());
    }
    string game_info = game_result_json(last_game_id, open_card_game_state, seed_str, wash_result,
                                               block_index, block_hash, allocations_log_info[0],
                                               allocations_log_info[1], allocations_log_info[2],
                                               allocations_log_info[3], allocations_log_info[4]);
    models::logs::insert(finger_official, game_info);
    return 0;
}

//结算接口
// settlement 缩写为slt
uint32_t bankbulls::sltgame(uint32_t dispose_count, int32_t count) {
    name finger_official = get_self();
    require_auth(finger_official);
    start_settlement(finger_official, dispose_count);
    return 0;
}

//8.7.1 轮次结算
// settlement 缩写为slt
//todo::dispose_count 用起来
uint32_t bankbulls::sltround(int32_t dispose_count ,uint32_t count) {
    name finger_official = get_self();
    require_auth(finger_official);
    //todo:根据upbankers表的total_divide来判断庄家是否已经进行了一次分红
    //todo::根据当前轮次表的总输赢和upbanker表的庄占比更新upbaner的分红更新
    vector <models::bankbulls::cur_round> cur_round_list = models::cur_round::list(finger_official);
    //当前轮次有且只有一个
    check(cur_round_list.size() == 1, "internal error cur_round_list size must be 1");
    vector <models::bankbulls::up_bankers> up_bankers_list = models::up_bankers::list(finger_official);
    check(up_bankers_list.size() > 0, "internal error, up bankers table have nothing");
    int64_t sum_total_divide = 0;
    for (uint32_t i = 0; i < up_bankers_list.size(); i++) {
        int64_t total_divide_amount = 0;
        int64_t total_divide_amount_nano = cur_round_list[0].banker_win_lose.amount * up_bankers_list[i].bet_ratio;
        /**
        //fixme::比例太小导致无法分摊的情况，如果直接取整的话，得到的数据是错乱的，先这样规避,(you)
        if((total_divide_amount_nano < 1000000 && total_divide_amount_nano > 0)
                || (total_divide_amount_nano > -1000000 && total_divide_amount_nano < 0)){
                printf("   3**%d   ",total_divide_amount_nano);
                printf("   4**%d   ",total_divide_amount_nano / 1000000);
                total_divide_amount_nano = 0;
        }**/
        //前面的玩家盈亏向下取整，最后一位兜底剩下的额度
        if (i == up_bankers_list.size() - 1) {
            total_divide_amount = cur_round_list[0].banker_win_lose.amount - sum_total_divide;
        } else {
            total_divide_amount = total_divide_amount_nano / 1000000;
            sum_total_divide += total_divide_amount;
        }
        //扣手续费,由于这个补偿，平台稍微有点让利0.0001eos
        total_divide_amount = total_divide_amount * 985 / 1000 + 1;
        models::up_bankers::update(finger_official, up_bankers_list[i].account, total_divide_amount);
    }
    //todo::加工信息推入历史轮次，清空当前轮次表，
    return 0;
}

//8.7.2 轮次开始
/**
 * todo：1,查询系统余额
        2,监听eosio.token 余额实时更新
        3,12分钟为配置项
        4,三个最大下注额度为配置项
 */
uint32_t bankbulls::startround() {
    print("startround ");
    name finger_official = get_self();
    require_auth(finger_official);
    //uint64_t last_round_id = models::cur_round::get_last_id(finger_official);
    vector <models::bankbulls::cur_round> cur_round_list = models::cur_round::list(finger_official);
    check(cur_round_list.size() <= 1, "internal error cur_round_list size must be 1");
    if (cur_round_list.size() != 0) {
        // fixme:由于当前轮次里面有个结算的状态因此，清空当前轮次，推入历史轮次的逻辑放在了轮次开始的时候
        insert_hisround_from_curound(finger_official, cur_round_list[0]);
    }
    uint64_t last_round_id = cur_round_list.size() == 0 ? 0 : cur_round_list[0].id;
    //  todo:轮次开始读取预约表中数据更新到当前轮次表里的player_banker_max_bet_amount，
    //先更新上庄表再更新当前轮次信息
    update_up_banker_and_settlement(finger_official);
    //在清空之前已经获取到上一轮的轮次id
    models::cur_round::clear(finger_official);
    update_cur_round(finger_official, last_round_id);
    printf("aaa =%d", last_round_id + 1);
    models::res_bankers::clear(finger_official);
    return 0;
}

//8.8预约接口
//action:预约动作:上庄：1,下庄：2,上调：3,下调：4
//预约动作：
//上庄：1
//下庄：2,此时的金额可有可无
//上调：3
//下调：4
//取消上庄：5，取消为预约上庄阶段的取消
uint32_t bankbulls::resbank(name account, asset investment, uint64_t action) {
    require_auth(account);
    eosio::action act = eosio::get_action(1, 0);
    printf(" action_name=%s ", act.name.to_string().data());
    if (act.name != name("transfer")) {
        if (action == up_banker_action || action == increase_banker_action) {
            check(false, "resbank action cann't be up_banker_action or increase_banker_action");
        }
    }

    name finger_official = get_self();
    uint32_t created_at = current_time_point().sec_since_epoch();
    // todo:下调和上调的金额检查
    switch (action) {
        case up_banker_action: {
            printf("up bankers");
            bool result = models::res_bankers::insert(finger_official, account, investment, action, created_at);
            check(result, "insert res_bankers table failed");
            break;
        }
        case down_bank_action:
        case increase_banker_action:
        case reduce_banker_action: {
            bool is_exist_result = models::up_bankers::is_exist(finger_official, account);
            check(is_exist_result, "account is not a up_bankers");
            bool insert_result = models::res_bankers::insert(finger_official, account, investment, action,
                                                             created_at);
            check(insert_result, "insert res_bankers table failed");
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
    return 0;
}

void bankbulls::testdelay() {
    printf("1");
    eosio::transaction txn{};
    string ss = "max_time";
    uint32_t bb = 3322;
    txn.actions.emplace_back(
            eosio::permission_level(_self, "active"_n),
            _self,
            "getdelay"_n, //action名称
            std::make_tuple(ss, 44432));//tuple
    txn.delay_sec = 1;
    txn.send(_self.value, _self, false);
}

void bankbulls::testdelaya() {
    printf("testdelayaaaa");
    string ss = "max_time";
    uint32_t bb = 33;
    action(permission_level{_self, name("active")},
           _self,
           name("getdelay"),
           std::make_tuple(ss))
            .send();
}

void bankbulls::getdelay(string key, uint32_t value) {
    printf("11%s", key.data());
    name finger_official = get_self();
    // require_auth(finger_official);
    auto table_key = STATE_KEY.find(key);
    //check(table_key != STATE_KEY.end(), "unknown key");
    printf("table_key is %d", table_key);
    //uint32_t value = 12567;
    bool result = models::global_state::insert(finger_official, table_key->second, value);
}

uint32_t bankbulls::tableclear(string table) {
    //name finger_official = get_self();
    //require_auth(finger_official);
    int64_t test1 = 1234;
    int64_t test2 = -1234;
    printf("   %d  ", test1 / 1000000);
    printf("   %d  ", test2 / 1000000);
    /**
    const models::bankbulls::cur_round *test = models::cur_round::get_last(get_self());
    if (test == NULL) {
        printf(" 1111 ");
    }
    printf(" !-%d  ", test->created_at);
    printf(" -%d  ", (*test).created_at);
     **/


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
    } else if (table == "all") {
        models::res_bankers::clear(finger_official);
        models::game::clear(finger_official);
        models::global_state::clear(finger_official);
        models::bets::clear(finger_official);
        models::up_bankers::clear(finger_official);
        models::his_round::clear(finger_official);
        models::cur_round::clear(finger_official);
    } else {
        check(false, "unknown params");
    }
    return 0;
}
