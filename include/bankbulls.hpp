#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/transaction.hpp>
//#include "eosio.token.hpp"
#include <eosio/crypto.hpp>
#include "type.hpp"

using namespace eosio;
using namespace std;

vector <string> split_string2(const string &s, const string &c) {
    vector <string> v;
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
    return v;
}

class [[eosio::contract]] bankbulls : public contract {
public:
    using contract::contract;

    bankbulls(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {
    }

    // [[eosio::action]] void playtest(string user_gestures,name user);
    [[eosio::action]] void playeos(name user, name to, asset quantity, string memo);

    // [[eosio::on_notify("eosio.token::transfer")]] void playeos(name user, name to, asset quantity, string memo);
    [[eosio::on_notify("eddytest1113::transfer")]] void play_finger(name user, name to, asset quantity, string memo);

    [[eosio::action]] bool setstate(string key, uint32_t value);

    [[eosio::action]] bool delstate(string key);


    // [[eosio::on_notify("eosio.token::transfer")]] uint32_t bet(name user, name to, asset quantity, string memo);
    uint32_t bet(name user, name to, asset quantity, string memo);

    [[eosio::action]] uint32_t startbet(uint64_t block_index, string sign);

    [[eosio::action]] void testsign(checksum256 &house_seed_hash, signature &sig, public_key &pubkey);

    [[eosio::action]] uint32_t stopbet();

    [[eosio::action]] void testdelay();

    [[eosio::action]] void testdelaya();

    [[eosio::action]] void getdelay(string player, uint32_t value);

    [[eosio::action]] uint32_t opencards(string sign, checksum256 seed, uint64_t block_index, string block_hash);
    // uint32_t bankbulls::opencards(signature &sign, checksum256 &seed,uint64_t block_index, string block_hash) {

    //[[eosio::action]] uint32_t opencards(string sign, checksum256 seed,uint64_t block_index, string block_hash);


    // settlement 缩写为slt
    //[[eosio::on_notify("eosio.token::transfer")]]
    [[eosio::action]] uint32_t sltgame(uint32_t dispose_count, int32_t count);

    [[eosio::action]] uint32_t sltround(int32_t dispose_count ,uint32_t count);

    [[eosio::action]] uint32_t startround();

    [[eosio::action]] uint32_t resbank(name account, asset investment, uint64_t action);
    //uint32_t resbank(name user, name to, asset quantity, string memo);

    [[eosio::action]] uint32_t tableclear(string table);


    //预约表中，监听预约上庄和加仓的事件，减仓和下庄和预约取消的为action
    [[eosio::on_notify("eosio.token::transfer")]] uint32_t
    notify_eosio_token(name user, name to, asset quantity, string memo) {
        if (user == get_self()) {
            return 0;
        }
        //todo：重点审计
        printf("memo---%s--%s-", memo.data(), to.to_string().data());
        vector <string> params = split_string2(memo, ",");
        string func = params[0];
        if (func == "bet") {
            bet(user, to, quantity, memo);
        } else if (func == "resbank") {
            uint32_t action = stoul(params[1]);
            //预约表中，监听预约上庄和加仓的事件，减仓和下庄和预约取消的为action
            if (action != up_banker_action && action != reduce_banker_action) {
                check(false, "action must be up banker or reduce banker");
            }
            resbank(user, quantity, action);
        } else {
            //check(false, "unknown func");
            //非法的memo直接没收币
            printf("unknown func");
        }
        return 0;
    }


    [[eosio::action]]

    void record(uint64_t game_id, uint32_t state, uint32_t dealer_hand_number,
                string dealer_card, string spades_card, string hearts_card,
                string clubs_card, string diamonds_card, int32_t spades_result,
                int32_t hearts_result, int32_t clubs_result, int32_t diamonds_result,
                uint32_t reveal_at, uint32_t created_at, uint64_t block_index, string block_hash,
                string seed, string seed_sign, string wash_hash
    ) {
        require_auth(get_self());
        return;
    }

    [[eosio::action]] void reveal(uint64_t game_id, name player, uint32_t dealer_hand_number,
                                  string dealer_card, string spades_card, string hearts_card,
                                  string clubs_card, string diamonds_card, asset spades_amt,
                                  asset hearts_amt, asset clubs_amt, asset diamonds_amt, asset bet_amount,
                                  asset payout_spades, asset payout_hearts, asset payout_clubs, asset payout_diamonds,
                                  uint32_t reveal_at, string seed, string seed_sign,
                                  string public_key, uint64_t block_id, string block_hash) {
        require_auth(get_self());
        return;
    }

// 只允许合约账户铸币
/**
[[eosio::action]] void create_finger()
{
    require_auth(get_self());
}


[[eosio::on_notify("*::transfer")]]
void transfer(name from, name to, asset quantity, string memo)
{
    //TODO something
}
**/
    TABLE game_records
            {
                    uint64_t id;
            name player;
            string user_gestures;
            string eos_gestures;
            string asset_name;
            uint64_t asset_amount; //这里不关注币种的单位
            int result;
            uint64_t playTime;

            uint64_t primary_key() const { return id; }
            uint64_t by_play_time() const { return -playTime; }
            };
    typedef eosio::multi_index<"bankbulls"_n, game_records, indexed_by<"time"_n,
            const_mem_fun < game_records, uint64_t, &game_records::by_play_time>>>
    records;
// 设置表
    TABLE global_state{
            uint64_t key; // u64?
            int32_t value;
            uint64_t primary_key() const { return key; }
    };
    typedef eosio::multi_index<"globalstate"_n, global_state> global_state_index;
// 游戏状态表
    TABLE game{
            uint64_t id;
            string sign;                   // 种子签名
            uint32_t state;                // 游戏状态
            // BYTE dealer_hand_number;	   // 庄家牌序号
            uint32_t dealer_hand_number;    // 庄家牌序号
            string dealer_card;            // 庄家牌显示
            string spades_card;            //spades家牌显示
            string hearts_card;            //hearts家牌显示
            string clubs_card;             // clubs家牌显示
            string diamonds_card;          //	diamonds家牌显示
            int32_t spades_result;         //	spades家牌结果
            int32_t hearts_result;         //	hearts家牌结果
            int32_t clubs_result;          //clubs家牌结果
            int32_t diamonds_result;       //	diamonds家牌结果
            uint32_t reveal_at;            //开牌时间
            uint64_t created_at;           //	创建时间
            uint64_t stop_at;              //停止下注时间
            uint64_t block_index;            //	区块高度todo:不用64
            string block_hash;             //	区块HASH
            string seed;                //游戏种子
            string wash_hash;            //	洗牌HASH
            uint64_t primary_key() const { return id; }
            uint64_t by_created_time() const { return -created_at; }
    };
    typedef eosio::multi_index<"games"_n, game, indexed_by<"createdat"_n,
            const_mem_fun < game, uint64_t, &game::by_created_time>>>
    game_index;

// 下注表
    TABLE bets{
            uint64_t id;                //PK自增长
            uint64_t game_id;        //游戏ID
            name player;                //玩家
            asset bet_amount;        //总下注额
            asset spades_amt;        //spades下注额
            asset hearts_amt;        //hearts下注额
            asset clubs_amt;            //clubs下注额
            asset diamonds_amt;        //diamonds下注额
            int32_t created_at;        //下注创建时间
            checksum256 tx_id;        //当前事务ID
            uint32_t state;        // 状态:待结算：1结算中：2已结算：3
            uint64_t primary_key() const { return id; }
            uint64_t by_created_time() const { return -created_at; }
            uint64_t by_game_id() const { return game_id; }
            uint64_t by_player() const { return player.value; }

    };
    typedef eosio::multi_index<"bets"_n, bets,
            indexed_by<"createdat"_n, const_mem_fun < bets, uint64_t, &bets::by_created_time>>,
    indexed_by<"gameid"_n, const_mem_fun<bets, uint64_t, &bets::by_game_id>>,
    indexed_by<"player"_n, const_mem_fun<bets, uint64_t, &bets::by_player>>
    >
    bets_index;

// 上庄表
    TABLE up_bankers{
            name account;            //玩家
            asset investment;        //投资额
            asset balance;            //余额
            asset total_divide;        //上庄分红总额
            uint32_t created_at;        //创建时间
            uint64_t bet_ratio;        //投资占比0.112066 = 112066
            uint64_t start_game_id;        //开始游戏ID
            uint64_t start_round_id;        //开始轮次ID
            uint64_t primary_key() const { return account.value; }
    };
    typedef eosio::multi_index<"upbankers"_n, up_bankers> up_bankers_index;

// 预约表
    TABLE res_bankers{
            name account;            //玩家
            asset investment;        //预约投资额
            uint32_t action;            //预约动作：上庄：1,下庄：2,上调：3,下调：4
            uint32_t created_at;        //创建时间
            uint64_t primary_key() const { return account.value; }
    };
    typedef eosio::multi_index<"resbankers"_n, res_bankers> res_bankers_index;

// 当前轮次表
    TABLE cur_round{
            uint64_t id;                            //PK 自增
            uint64_t cur_game_id;                    //当前游戏ID
            asset player_investment;            //玩家投资额
            asset system_investment;            //系统投资额
            asset player_banker_balance;        //玩家余额
            asset system_banker_balance;        //系统余额
            asset player_banker_max_bet_amount;        //玩家上庄最大投注额
            asset system_banker_max_bet_amount;        //系统上庄最大投注额
            asset cur_max_bet;                    //单人投注最大额
            uint32_t banker_num;                    //上庄总人数
            asset spades_amt;                    //spades总下注额
            asset hearts_amt;                    //hearts总下注额
            asset clubs_amt;                    //clubs总下注额
            asset diamonds_amt;                    //diamonds总下注额
            asset current_banker_win;            //庄家赢总和
            asset current_banker_lose;            //庄家输总和
            asset banker_win_lose;                //庄家赢输总和
            asset player_banker_bet;            //玩家下注总和
            uint32_t state;                        //轮次状态:正常：1,结束：2
            uint32_t created_at;                    //创建时间
            uint32_t stop_at;                    //下个轮次时间
            uint64_t primary_key() const { return id; }
            uint64_t by_created_time() const { return -created_at; }
    };
    typedef eosio::multi_index<"curround"_n, cur_round, indexed_by<"createdat"_n,
            const_mem_fun < cur_round, uint64_t, &cur_round::by_created_time>>>
    cur_round_index;
// 历史轮次表
    TABLE his_rounds{
            uint64_t id;                //轮次ID
            uint64_t start_game_id;    //	开始游戏ID
            uint64_t end_game_id;        //结束游戏ID
            asset bank_investment;    //	庄投资额
            asset bank_balance;        //庄余额
            asset bank_profit;        //盈利
            asset sys_investment;    //	系统投资额
            asset sys_balance;        //系统余额
            asset sys_profit;        //系统盈利
            uint64_t created_at;        //创建时间
            uint64_t end_time;            //结束时间
            uint64_t primary_key() const { return id; }
            uint64_t by_created_time() const { return -created_at; }
    };
    typedef eosio::multi_index<"hisrounds"_n, his_rounds, indexed_by<"createdat"_n,
            const_mem_fun < his_rounds, uint64_t, &his_rounds::by_created_time>>>
    his_rounds_index;


    TABLE logs{
            uint64_t id;
            string data;
            uint64_t primary_key() const { return -id; }
    };
    typedef eosio::multi_index<"logs"_n, logs> logs_index;

    TABLE logs2{
            uint64_t id;
            string data;
            uint64_t primary_key() const { return -id; }
    };
    typedef eosio::multi_index<"logs2"_n, logs2> logs2_index;

};
