#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/transaction.hpp>
//#include "eosio.token.hpp"
#include <eosio/crypto.hpp>
//#include "bankbulls_utils.hpp"


using namespace eosio;
using namespace std;
namespace models {

#include "bankbulls.hpp"

    class global_state {
        //string key, uint32_t value
    public:
        static bool insert(name contract, uint64_t key, uint32_t value) {
            bankbulls::global_state_index global_state_table(contract, contract.value);
            auto itr = global_state_table.find(key);
            if (itr != global_state_table.end()) {
                check(false, "key already exist");
            }
            global_state_table.emplace(contract, [&](auto &u) {
                u.key = key;
                u.value = value;
            });
            return true;
        }

        static bool clear(name contract) {
            bankbulls::global_state_index global_state_table(contract, contract.value);
            for (auto it = global_state_table.begin(); it != global_state_table.end();) {
                it = global_state_table.erase(it);
            }
            return true;
        }

        static bool remove(name contract, uint64_t key) {
            bankbulls::global_state_index global_state_table(contract, contract.value);
            auto itr = global_state_table.find(key);
            if (itr == global_state_table.end()) {
                check(false, "user does not exist in table, nothing to delete");
            }
            global_state_table.erase(itr);
            return true;
        }
    };

    class game {
    public:
        /*
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
            uint64_t block_index;            //	区块高度
            string block_hash;             //	区块HASH
            string seed;                //游戏种子
            string wash_hash;            //	洗牌HASH
    }
         * */
        static bool insert(name contract, uint64_t id, string sign, uint32_t state, uint32_t dealer_hand_number,
                           string dealer_card, string spades_card, string hearts_card, string clubs_card,
                           string diamonds_card, int32_t spades_result, int32_t hearts_result, int32_t clubs_result,
                           int32_t diamonds_result, uint32_t reveal_at, uint64_t created_at, uint64_t stop_at,
                           uint64_t block_index, string block_hash, string seed, string wash_hash) {

            bankbulls::game_index game_table(contract, contract.value);
            auto itr = game_table.find(id);
            if (itr != game_table.end()) {
                check(false, "game id already exist");
            }
            game_table.emplace(contract, [&](auto &u) {
                u.id = id;
                u.sign = sign;
                u.state = state;
                u.dealer_hand_number = dealer_hand_number;
                u.dealer_card = dealer_card;
                u.spades_card = spades_card;
                u.hearts_card = hearts_card;
                u.clubs_card = clubs_card;
                u.diamonds_card = diamonds_card;
                u.spades_result = spades_result;
                u.hearts_result = hearts_result;
                u.clubs_result = clubs_result;
                u.diamonds_result = diamonds_result;
                u.reveal_at = reveal_at;
                u.created_at = created_at;
                u.stop_at = stop_at;
                u.block_index = block_index;
                u.block_hash = block_hash;
                u.seed = seed;
                u.wash_hash = wash_hash;
            });
            return true;
        }

        static bool remove() {
            return true;
        }

        static bool update(name contract, uint64_t id, uint64_t state) {
            bankbulls::game_index game_table(contract, contract.value);
            auto itr = game_table.find(id);
            if (itr == game_table.end()) {
                check(false, "have no this game id");
            }
            game_table.modify(itr, contract, [&](auto &u) {
                u.state = state;
                //fixme::stop_time要不要更新？用开局时候计算的还是stop时候的时间
            });
            return true;
        }

        //开牌时候更新相关入参
        static bool update(name contract, uint64_t id, uint64_t state, uint32_t reveal_at,
                           string seed, string block_hash) {
            bankbulls::game_index game_table(contract, contract.value);
            auto itr = game_table.find(id);
            if (itr == game_table.end()) {
                check(false, "have no this game id");
            }
            game_table.modify(itr, contract, [&](auto &u) {
                u.state = state;
                u.reveal_at = reveal_at;
                u.seed = seed;
                u.block_hash = block_hash;
            });
            return true;
        }

        //更新开牌结果
        static bool update(name contract, uint64_t id, vector <string> allocation_infos, vector <int32_t> bet_results,
                           uint32_t last_char_hex, string wash_hash) {
            bankbulls::game_index game_table(contract, contract.value);
            auto itr = game_table.find(id);
            if (itr == game_table.end()) {
                check(false, "have no this game id");
            }
            game_table.modify(itr, contract, [&](auto &u) {
                u.dealer_hand_number = last_char_hex;
                u.dealer_card = allocation_infos[0];
                u.spades_card = allocation_infos[1];
                u.hearts_card = allocation_infos[2];
                u.clubs_card = allocation_infos[3];
                u.diamonds_card = allocation_infos[4];
                u.spades_result = bet_results[0];
                u.hearts_result = bet_results[1];
                u.clubs_result = bet_results[2];
                u.diamonds_result = bet_results[3];
                u.wash_hash = wash_hash;
            });
            return true;
        }

        static bankbulls::game find(name contract, uint64_t id) {
            bankbulls::game_index game_table(contract, contract.value);
            auto itr = game_table.find(id);
            return *itr;
        }

        static bool clear(name contract) {
            bankbulls::game_index game_table(contract, contract.value);
            for (auto it = game_table.begin(); it != game_table.end();) {
                it = game_table.erase(it);
            }
            return true;
        }

        static uint64_t get_last_id(name account) {
            bankbulls::game_index game_table(account, account.value);
            auto created_at_idx = game_table.get_index<"createdat"_n>();
            auto last_itr = created_at_idx.begin();
            if (last_itr == created_at_idx.end()) {
                printf("have nothing in table");
                return 0;
            }
            return last_itr->id;
        }
    };

    class bets {
    public:
        static bool
        insert(name contract, uint64_t id, uint64_t game_id, name player, asset bet_amount, asset spades_amt,
               asset hearts_amt, asset clubs_amt, asset diamonds_amt, int32_t created_at,
               checksum256 tx_id, uint32_t state) {
            bankbulls::bets_index bets_table(contract, contract.value);
            auto itr = bets_table.find(id);
            if (itr != bets_table.end()) {
                check(false, "bets id already exist");
            }
            bets_table.emplace(contract, [&](auto &u) {
                u.id = id;
                u.game_id = game_id;
                u.player = player;
                u.bet_amount = bet_amount;
                u.spades_amt = spades_amt;
                u.hearts_amt = hearts_amt;
                u.clubs_amt = clubs_amt;
                u.diamonds_amt = diamonds_amt;
                u.created_at = created_at;
                u.tx_id = tx_id;
                u.state = state;
            });
            return true;
        }

        static bool remove() {
            return true;
        }

        static bool update(name contract, uint64_t id, uint64_t state) {
            bankbulls::bets_index bets_table(contract, contract.value);
            auto itr = bets_table.find(id);
            if (itr == bets_table.end()) {
                check(false, "have no this game id");
            }
            bets_table.modify(itr, contract, [&](auto &u) {
                u.state = state;
            });
            return true;
        }

        static bool find() {
            return true;
        }


        static vector <bankbulls::bets> list(name account, uint64_t game_id, uint32_t state) {
            vector <bankbulls::bets> bets_list;
            bankbulls::bets_index bets_table(account, account.value);
            /**
            for (const auto &item : bets_table) {
                printf(" ID1=%d ", item.id);
            }**/
            auto game_id_idx = bets_table.get_index<"gameid"_n>();
            auto itr = game_id_idx.find(game_id);
            for (auto it = game_id_idx.begin(); it != game_id_idx.end(); it++) {
                if (it->state == state) {
                    bets_list.push_back(*it);
                }
            }
            return bets_list;
        }

        static bool clear(name contract) {
            bankbulls::bets_index bets_table(contract, contract.value);
            for (auto it = bets_table.begin(); it != bets_table.end();) {
                it = bets_table.erase(it);
            }
            return true;
        }

        static uint64_t get_last_id(name account) {
            bankbulls::bets_index bets_table(account, account.value);
            auto created_at_idx = bets_table.get_index<"createdat"_n>();
            auto last_itr = created_at_idx.begin();
            if (last_itr == created_at_idx.end()) {
                printf("have nothing in table");
                return 0;
            }
            return last_itr->id;
        }
    };

    class up_bankers {
    public:
        /*
         *   TABLE up_bankers{
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
         * */
        static bool insert(name contract, name user, asset investment, asset balance, asset total_divide,
                           uint32_t created_at, uint64_t bet_ratio, uint64_t start_game_id, uint64_t start_round_id) {
            bankbulls::up_bankers_index up_bankers_table(contract, contract.value);
            auto itr = up_bankers_table.find(user.value);
            if (itr != up_bankers_table.end()) {
                check(false, "user have been exist already");
            }
            up_bankers_table.emplace(contract, [&](auto &u) {
                u.account = user;
                u.investment = investment;
                u.balance = balance;
                u.total_divide = total_divide;
                u.created_at = created_at;
                u.bet_ratio = bet_ratio;
                u.start_game_id = start_game_id;
                u.start_round_id = start_round_id;
            });
            return true;
        }

        static bool remove(name contract, name user) {
            bankbulls::up_bankers_index up_banker_table(contract, contract.value);
            auto itr = up_banker_table.find(user.value);
            if (itr == up_banker_table.end()) {
                check(false, "user not found");
            }
            up_banker_table.erase(itr);
            return true;
        }

        static bool clear(name contract) {
            bankbulls::up_bankers_index up_banker_table(contract, contract.value);
            for (auto it = up_banker_table.begin(); it != up_banker_table.end();) {
                it = up_banker_table.erase(it);
            }
            return true;
        }


        static vector <bankbulls::up_bankers> list(name contract) {
            vector <bankbulls::up_bankers> up_banker_list;
            bankbulls::up_bankers_index up_banker_table(contract, contract.value);
            for (const auto &item : up_banker_table) {
                up_banker_list.push_back(item);
            }
            return up_banker_list;
        }

        //轮次换庄家更新投资额
        static bool update(name contract, name user, asset investment) {
            bankbulls::up_bankers_index up_bankers_table(contract, contract.value);
            auto itr = up_bankers_table.find(user.value);
            if (itr == up_bankers_table.end()) {
                check(false, "user not found");
            }
            up_bankers_table.modify(itr, contract, [&](auto &u) {
                u.investment = investment;
            });
            return true;
        }

        //轮次换庄家更新投资占比，余额等
        static bool update(name contract, name user, asset balance, asset total_divide,
                           uint32_t created_at, uint64_t bet_ratio, uint64_t start_game_id, uint64_t start_round_id) {
            bankbulls::up_bankers_index up_bankers_table(contract, contract.value);
            auto itr = up_bankers_table.find(user.value);
            if (itr == up_bankers_table.end()) {
                check(false, "user not found");
            }
            up_bankers_table.modify(itr, contract, [&](auto &u) {
                u.balance = balance;
                u.total_divide = total_divide;
                u.created_at = created_at;
                u.bet_ratio = bet_ratio;
                u.start_game_id = start_game_id;
                u.start_round_id = start_round_id;
            });
            return true;
        }

        //todo:每轮结算更新分红
        static bool update(name contract, name user, int64_t total_divide_amount) {
            bankbulls::up_bankers_index up_bankers_table(contract, contract.value);
            auto itr = up_bankers_table.find(user.value);
            if (itr == up_bankers_table.end()) {
                check(false, "user not found");
            }
            up_bankers_table.modify(itr, contract, [&](auto &u) {
                u.total_divide.amount = total_divide_amount;
            });
            return true;
        }

        /**
        //todo:系统补庄，更新投资占比
        static bool update(name contract,name user,uint64_t bet_ratio) {
            bankbulls::up_bankers_index up_bankers_table(contract, contract.value);
            auto itr = up_bankers_table.find(user.value);
            if (itr == up_bankers_table.end()) {
                check(false,"user not found");
            }
            up_bankers_table.modify(itr, contract, [&](auto &u) {
                u.bet_ratio = bet_ratio;
            });
            return true;
        }**/

        static bool is_exist(name contract, name user) {
            bankbulls::up_bankers_index up_bankers_table(contract, contract.value);
            auto itr = up_bankers_table.find(user.value);
            if (itr == up_bankers_table.end()) {
                return false;
            }
            return true;
        }
    };

    class res_bankers {
        /*
         *   TABLE res_bankers{
            name account;            //玩家
            asset investment;        //预约投资额
            uint32_t action;            //预约动作：上庄：1,下庄：2,上调：3,下调：4
            uint32_t created_at;        //创建时间
            uint64_t primary_key() const { return account.value; }
    };
        上庄insert，下庄和调整都是update，该表等下一轮次开始后清空
         * */
    public:
        static bool insert(name contract, name user, asset investment, uint32_t action, uint32_t created_at) {
            bankbulls::res_bankers_index res_bankers_table(contract, contract.value);
            auto itr = res_bankers_table.find(user.value);
            if (itr != res_bankers_table.end()) {
                check(false, "user have been exist already");
            }
            res_bankers_table.emplace(contract, [&](auto &u) {
                u.account = user;
                u.investment = investment;
                u.action = action;
                u.created_at = created_at;
            });
            return true;
        }

        static bool find() {
            return true;
        }

        static vector <bankbulls::res_bankers> list(name contract) {
            vector <bankbulls::res_bankers> res_banker_list = {};
            bankbulls::res_bankers_index res_banker_table(contract, contract.value);
            for (const auto &item : res_banker_table) {
                res_banker_list.push_back(item);
            }
            return res_banker_list;
        }

        static bool clear(name contract) {
            bankbulls::res_bankers_index res_bankers_table(contract, contract.value);
            for (auto it = res_bankers_table.begin(); it != res_bankers_table.end();) {
                it = res_bankers_table.erase(it);
            }
            return true;
        }

        static bool remove(name contract, name user) {
            bankbulls::res_bankers_index res_bankers_table(contract, contract.value);
            auto itr = res_bankers_table.find(user.value);
            if (itr == res_bankers_table.end()) {
                check(false, "user not found");
            }
            res_bankers_table.erase(itr);
            return true;
        }
    };

    //asset cur_max_bet		            //单人投注最大额
    //            uint32 banker_num		            //上庄总人数
    class cur_round {
    public:
        static bool insert(name contract, uint64_t id, uint64_t cur_game_id, asset player_investment,
                           asset system_investment, asset player_banker_balance, asset system_banker_balance,
                           asset player_banker_max_bet_amount, asset system_banker_max_bet_amount, asset cur_max_bet,
                           uint32_t banker_num, asset spades_amt, asset hearts_amt, asset clubs_amt, asset diamonds_amt,
                           asset current_banker_win, asset current_banker_lose, asset banker_win_lose,
                           asset player_banker_bet, uint32_t state, uint32_t created_at, uint32_t stop_at) {
            bankbulls::cur_round_index cur_round_table(contract, contract.value);
            auto itr = cur_round_table.find(id);
            if (itr != cur_round_table.end()) {
                check(false, "cur_round id have been exist already");
            }
            cur_round_table.emplace(contract, [&](auto &u) {
                u.id = id;                            //PK 自增
                u.cur_game_id = cur_game_id;                    //当前游戏ID
                u.player_investment = player_investment;            //玩家投资额
                u.system_investment = system_investment;            //系统投资额
                u.player_banker_balance = player_banker_balance;        //玩家余额
                u.system_banker_balance = system_banker_balance;        //系统余额
                u.player_banker_max_bet_amount = player_banker_max_bet_amount;        //玩家上庄最大投注额
                u.system_banker_max_bet_amount = system_banker_max_bet_amount;        //系统上庄最大投注额
                //u.reserve_amount = reserve_amount;                //储备金
                u.cur_max_bet = cur_max_bet;
                u.banker_num = banker_num;
                u.spades_amt = spades_amt;                    //spades总下注额
                u.hearts_amt = hearts_amt;                    //hearts总下注额
                u.clubs_amt = clubs_amt;                    //clubs总下注额
                u.diamonds_amt = diamonds_amt;                    //diamonds总下注额
                u.current_banker_win = current_banker_win;            //庄家赢总和
                u.current_banker_lose = current_banker_lose;            //庄家输总和
                u.banker_win_lose = banker_win_lose;                //庄家赢输总和
                u.player_banker_bet = player_banker_bet;            //玩家下注总和
                u.state = state;                        //轮次状态:正常：1,结束：2
                u.created_at = created_at;                    //创建时间
                u.stop_at = stop_at;                    //下个轮次时间
            });
            return true;
        }

        static bool remove() {
            return true;
        }

        static vector <bankbulls::cur_round> list(name contract) {
            vector <bankbulls::cur_round> cur_round_list = {};
            bankbulls::cur_round_index cur_round_table(contract, contract.value);
            for (const auto &item : cur_round_table) {
                cur_round_list.push_back(item);
            }
            return cur_round_list;
        }

        //更新当前游戏id
        static bool update(name contract, uint64_t round_id, uint64_t cur_game_id) {
            bankbulls::cur_round_index cur_round_table(contract, contract.value);
            auto itr = cur_round_table.find(round_id);
            if (itr == cur_round_table.end()) {
                check(false, "cur_round not found");
            }
            cur_round_table.modify(itr, contract, [&](auto &u) {
                u.cur_game_id = cur_game_id;
            });
            return true;
        }

        //todo:下注更新下注总额
        static bool update(name contract, uint64_t round_id, asset bet_amount,
                           asset spades_amt, asset hearts_amt, asset clubs_amt, asset diamonds_amt) {
            bankbulls::cur_round_index cur_round_table(contract, contract.value);
            auto itr = cur_round_table.find(round_id);
            if (itr == cur_round_table.end()) {
                check(false, "round_id not found when update");
            }
            cur_round_table.modify(itr, contract, [&](auto &u) {
                u.player_banker_bet.amount += bet_amount.amount;
                u.spades_amt.amount += spades_amt.amount;                    //spades总下注额
                u.hearts_amt.amount += hearts_amt.amount;                    //hearts总下注额
                u.clubs_amt.amount += clubs_amt.amount;                    //clubs总下注额
                u.diamonds_amt.amount += diamonds_amt.amount;
            });
            return true;
        }
        //todo:结算更新输赢总额
        //contract,last_cur_round_id,current_banker_win,current_banker_lose,banker_win_lose

        static bool update(name contract, uint64_t round_id, int64_t current_banker_win_amount,
                           int64_t current_banker_lose_amount, int64_t banker_win_lose_amount) {
            bankbulls::cur_round_index cur_round_table(contract, contract.value);
            auto itr = cur_round_table.find(round_id);
            if (itr == cur_round_table.end()) {
                check(false, "round_id not found when update");
            }
            cur_round_table.modify(itr, contract, [&](auto &u) {
                u.current_banker_win.amount += current_banker_win_amount;
                u.current_banker_lose.amount += current_banker_lose_amount;
                u.banker_win_lose.amount += banker_win_lose_amount;
            });
            return true;
        }

        static bool find() {
            return true;
        }

        static bool clear(name contract) {
            bankbulls::cur_round_index cur_round_table(contract, contract.value);
            for (auto it = cur_round_table.begin(); it != cur_round_table.end();) {
                it = cur_round_table.erase(it);
            }
            return true;
        }


        static uint64_t get_last_id(name account) {
            bankbulls::cur_round_index cur_round_table(account, account.value);
            auto created_at_idx = cur_round_table.get_index<"createdat"_n>();
            auto last_itr = created_at_idx.begin();
            if (last_itr == created_at_idx.end()) {
                printf("have nothing in table");
                return 0;
            }
            return last_itr->id;
        }

        static vector <bankbulls::cur_round> list_by_created(name contract) {
            vector <bankbulls::cur_round> cur_round_list = {};
            bankbulls::cur_round_index cur_round_table(contract, contract.value);
            auto created_at_idx = cur_round_table.get_index<"createdat"_n>();
            for (const auto &item : created_at_idx) {
                cur_round_list.push_back(item);
            }
            return cur_round_list;
        }

    };

    class his_round {
    public:
        /**
         *   TABLE his_rounds{
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
         *
         * **/
        static bool insert(name contract, uint64_t id, uint64_t start_game_id, uint64_t end_game_id,
                           asset bank_investment, asset bank_balance, asset bank_profit,
                           asset sys_investment, asset sys_balance, asset sys_profit,
                           uint64_t created_at, uint64_t end_time) {

            bankbulls::his_rounds_index his_rounds_table(contract, contract.value);
            auto itr = his_rounds_table.find(id);
            if (itr != his_rounds_table.end()) {
                check(false, "round id is already exist");
            }
            his_rounds_table.emplace(contract, [&](auto &u) {
                u.id = id;
                u.start_game_id = start_game_id;
                u.end_game_id = end_game_id;
                u.bank_investment = bank_investment;
                u.bank_balance = bank_balance;
                u.bank_profit = bank_profit;
                u.sys_investment = sys_investment;
                u.sys_balance = sys_balance;
                u.sys_profit = sys_profit;
                u.created_at = created_at;
                u.end_time = end_time;
            });
            return true;
        }

        static bool remove() {
            return true;
        }

        static bool update() {
            return true;
        }

        static bool find() {
            return true;
        }

        static bool clear(name contract) {
            bankbulls::his_rounds_index his_rounds_table(contract, contract.value);
            for (auto it = his_rounds_table.begin(); it != his_rounds_table.end();) {
                it = his_rounds_table.erase(it);
            }
            return true;
        }

        static vector <bankbulls::his_rounds> get_last(name account) {
            bankbulls::his_rounds_index his_rounds_table(account, account.value);
            auto created_at_idx = his_rounds_table.get_index<"createdat"_n>();
            vector <bankbulls::his_rounds> rounds;
            auto last_itr = created_at_idx.begin();
            if (last_itr == created_at_idx.end()) {
                printf("have nothing in table");
                return {};
            }
            rounds.push_back(*last_itr);
            return rounds;
        }
    };


    class logs {
    public:
        static bool insert(name contract,string data) {

            vector<bankbulls::logs> logs = list(contract);
            uint64_t id =  logs.size() == 0 ? 1 : logs[0].id + 1;

            bankbulls::logs_index logs_table(contract, contract.value);
            auto itr = logs_table.find(id);
            if (itr != logs_table.end()) {
                check(false, "logs id is already exist");
            }
            logs_table.emplace(contract, [&](auto &u) {
                u.id = id;
                u.data = data;
            });
            return true;
        }

        static bool clear(name contract) {
            bankbulls::logs_index logs_table(contract, contract.value);
            for (auto it = logs_table.begin(); it != logs_table.end();) {
                it = logs_table.erase(it);
            }
            return true;
        }

        static vector <bankbulls::logs> list(name contract) {
            bankbulls::logs_index logs_table(contract, contract.value);
            vector <bankbulls::logs> logs = {};
            auto last_itr = logs_table.begin();
            for (const auto &item : logs_table) {
                logs.push_back(item);
            }
            return logs;
        }
    };


    class logs2 {
    public:
        static bool insert(name contract,string data) {

            vector<bankbulls::logs2> logs = list(contract);
            uint64_t id =  logs.size() == 0 ? 1 : logs[0].id + 1;

            bankbulls::logs2_index logs_table(contract, contract.value);
            auto itr = logs_table.find(id);
            if (itr != logs_table.end()) {
                check(false, "logs id is already exist");
            }
            logs_table.emplace(contract, [&](auto &u) {
                u.id = id;
                u.data = data;
            });
            return true;
        }

        static bool clear(name contract) {
            bankbulls::logs2_index logs_table(contract, contract.value);
            for (auto it = logs_table.begin(); it != logs_table.end();) {
                it = logs_table.erase(it);
            }
            return true;
        }

        static vector <bankbulls::logs2> list(name contract) {
            bankbulls::logs2_index logs_table(contract, contract.value);
            vector <bankbulls::logs2> logs = {};
            auto last_itr = logs_table.begin();
            for (const auto &item : logs_table) {
                logs.push_back(item);
            }
            return logs;
        }
    };
}

