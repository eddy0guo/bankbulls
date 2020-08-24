/*
banker bull log
*/
#include "nlohmann/json.hpp"

using namespace eosio;
using namespace std;


/*
 *
 * JSON串规范：字段都是表中字段


6.本局结算完成
{
 	"event_id":"gameSettlementFinish",
"game_id":1233,
"state":5
}

7.本轮结算中
{
 	"event_id":"roundSettlementing",
"round_id":1233,
}

8.本轮结算完成
{
 	"event_id":"roundSettlementingFinish",
"round_id":1233,
"banker_payout_total":600
}

9.下轮开始
{
 	"event_id":"roundBegin",
"round_id":1234
}
 * */
/**
 *
 * 1.游戏开局
{
    "event_id":"gameStart",
    "game_id":1233,
    "state":1,
"stop_at":15932424434,
"sign":"SIG_K1_x",
"block_index":1232344,
"round_id":5762,
"round_stop_at":15932424434,
"player_banker_balance":11221321.1233,
"system_banker_balance":11221321.1233,
"player_banker_max_bet_amount":11221321.1233,
"system_banker_max_bet_amount":1321.1233,
"banker_num":136,
"cur_max_bet":600
}
 * */
std::string game_start_json(uint64_t game_id, uint32_t state, uint32_t stop_at, string sign,
                            uint32_t block_index, uint32_t round_id, uint32_t round_stop_at,
                            string player_banker_balance, string system_banker_balance,
                            string player_banker_max_bet_amount, string system_banker_max_bet_amount,
                            uint32_t banker_num, string cur_max_bet) {
    nlohmann::json tmp_json;
    tmp_json["event_id"] = "gameStart";
    tmp_json["game_id"] = game_id;
    tmp_json["state"] = state;
    tmp_json["stop_at"] = stop_at;
    tmp_json["sign"] = sign;
    tmp_json["block_index"] = block_index;
    tmp_json["round_id"] = round_id;
    tmp_json["round_stop_at"] = round_stop_at;
    tmp_json["player_banker_balance"] = player_banker_balance;
    tmp_json["system_banker_balance"] = system_banker_balance;
    tmp_json["player_banker_max_bet_amount"] = player_banker_max_bet_amount;
    tmp_json["system_banker_max_bet_amount"] = system_banker_max_bet_amount;
    tmp_json["banker_num"] = banker_num;
    tmp_json["cur_max_bet"] = cur_max_bet;
    return tmp_json.dump();
}


/*
 * 2.玩家下注
{
    "event_id":"newBet",
    "game_id":1233,
    "player":"sdfdsfds",
    "spades_amt":1,
    "hearts_amt":2,
"clubs_amt":0.5,
"diamonds_amt":0.5,
"created_at":159087364
}
 * */
std::string new_bet_json(uint64_t game_id, string player, string spades_amt, string hearts_amt,
                            string clubs_amt, string diamonds_amt, uint32_t created_at) {
    nlohmann::json tmp_json;
    tmp_json["event_id"] = "newBet";
    tmp_json["game_id"] = game_id;
    tmp_json["player"] = player;
    tmp_json["spades_amt"] = spades_amt;
    tmp_json["hearts_amt"] = hearts_amt;
    tmp_json["clubs_amt"] = clubs_amt;
    tmp_json["diamonds_amt"] = diamonds_amt;
    tmp_json["created_at"] = created_at;
    return tmp_json.dump();
}
/**
3.停止下注
{
"event_id":"gameStop",
"game_id":1233,
"state":2
}
 **/

std::string stop_bet_json(uint64_t game_id,uint32_t state) {
    nlohmann::json tmp_json;
    tmp_json["event_id"] = "gameStop";
    tmp_json["game_id"] = game_id;
    tmp_json["state"] = state;
    return tmp_json.dump();
}

/**
 * 4.游戏开牌
牌串说明：
23|7|38|57|8|5|8|-1
前5个是牌值：23|7|38|57|8
第6个是选牌序号：5
第7个是牌型，见牌型枚举值：8
第8个是牌结果：-1
庄无结果值即0，见游戏表中的spades_result，输赢倍数
{
    "event_id":"gameResult",
    "game_id":1233,
"state":3,
"seed":"xxxxxxxx232342432423424xxxxxxx",
    "wash_hash":"xxxxxxxx232342432423424xxxxxxx",
    "block_index":12342443,
"block_hash":"xxxx232342432423424xxxxx",
    "dealer_card":"23|7|38|57|8|4|8|0",
"spades_card":"23|7|38|57|8|5|8|-1",
"hearts_card":"23|7|38|57|8|1|4|-1",
"clubs_card":"23|7|38|57|8|8|2|5|-1",
"diamonds_card":"23|7|38|57|3|10|2"
}
 * */


std::string game_result_json(uint64_t game_id,uint32_t state,string seed,string wash_hash,
                             uint32_t block_index,string block_hash,string dealer_card,
                             string spades_card,string hearts_card, string clubs_card,
                             string diamonds_card ) {
    nlohmann::json tmp_json;
    //todo::这里eosio内存分配异常，先带上下标1
    tmp_json["event_id1"] = "gameResult";
    tmp_json["game_id1"] = game_id;
    tmp_json["state1"] = state;
    tmp_json["seed"] = seed;tmp_json["wash_hash"] = wash_hash;
    tmp_json["block_index"] = block_index;
    tmp_json["block_hash"] = block_hash;
    tmp_json["dealer_card"] = dealer_card;
    tmp_json["spades_card"] = spades_card;
    tmp_json["hearts_card"] = hearts_card;
    tmp_json["clubs_card"] = clubs_card;
    tmp_json["diamonds_card"] = diamonds_card;
    return tmp_json.dump();
}

/**
5.本局结算
{
"event_id":"gameSettlement",
"game_id":1233,
"state":4,
"banker_payout":600,
"winners":[
}
**/
//todo：所有类型都在log.hpp里转
std::string player_winners_json(name player,asset win_amt) {
    nlohmann::json tmp_json;
    tmp_json["player"] = player.to_string();
    tmp_json["win_amt"] = win_amt.to_string();
    return tmp_json.dump();
}

std::string game_settlement_json(uint64_t game_id,uint32_t state,asset banker_payout,vector<string> winners) {
    nlohmann::json tmp_json;
    tmp_json["event_id"] = "gameSettlement";
    tmp_json["game_id"] = game_id;
    tmp_json["state"] = state;
    tmp_json["banker_payout"] = banker_payout.to_string();
    //todo:array 不是数组
    tmp_json["winners"] = winners;
    return tmp_json.dump();
}