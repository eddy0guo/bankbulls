#pragma once

#include "nlohmann/json.hpp"
#include<tuple>
#include "bankbulls.hpp"


using namespace eosio;
using namespace std;

#define EOS_SYMBOL symbol(symbol_code("EOS"),4)
#define DEFAULT_ASSET asset(0, EOS_SYMBOL)
#define FINGER_SYMBOL symbol(symbol_code("FINGER"),4)

//table name
#define LOGS2_TABLE_NAME  "logs2"
#define LOGS_TABLE_NAME  "logs"
#define HIS_ROUNDS_TABLE_NAME  "hisrounds"
#define CUR_ROUND_TABLE_NAME  "curround"
#define RES_BANKERS_TABLE_NAME  "resbankers"
#define UP_BANKERS_TABLE_NAME  "upbankers"
#define BETS2_TABLE_NAME  "bets2"
#define GAMES_TABLE_NAME  "games"
#define SETTINGS_TABLE_NAME "globalstate3"

//keyname
#define TIME_STOP_KEY_NAME "timestop"
#define TIME_GAME_KEY_NAME "timegame"
#define TIME_ROUND_KEY_NAME "timeround"
#define MIN_UP_BANK_KEY_NAME "minupbank"
#define MIN_DOWN_BANK_KEY_NAME "mindownbank"
#define LIMIT_PLAY_BANK_KEY_NAME "limitplybank"
#define LIMIT_SYSTEM_BANK_KEY_NAME "limitsysbank"
#define SIZE_GAMES_KEY_NAME "sizegames"
#define SIZE_ROUNDS_KEY_NAME "sizerounds"
#define LAST_LOGS2_ID_KEY_NAME "lastlogs2id"
//fixme:放进设置项还没给join说
#define MIN_INCREASE_KEY_NAME "minplusbank"
#define MIN_REDUCE_KEY_NAME "minminusbank"
#define SETTLEMENT_SIZE_KEY_NAME "sltsize"

//组合type,组合名称,最大点数，最大点数对应的花色，赔率，
typedef std::tuple<uint32_t, string, uint32_t, uint32_t, uint32_t> ALLOCATION_INFO;
typedef std::tuple<int32_t, string> BET_RESULT;

//SID = sender id
const uint128_t TRANSFER_SID = name("eosio.token").value + name("transfer").value; //2294284288
const uint128_t SLT_ROUND_SID = name("sltround").value; //1761607680
const uint128_t SLT_GAME_SID = name("sltgame").value; //1073741824
const uint128_t START_ROUND_SID = name("startround").value; //1701167104
const uint128_t STOP_BET_SID = name("stopbet").value; //536870912


const string SPADES_ICON = "♠";
const string HEARTS_ICON = "♥";
const string CLUBS_ICON = "♣";
const string DIAMONDS_ICON = "♦";

const map<uint32_t, string> CARD_TYPE_ICON = {
        {3, "♠"},
        {2, "♥"},
        {1, "♣"},
        {0, "♦"}
};
const map<uint32_t, string> CARD_NUMBER_ICON = {
        {13, "K"},
        {12, "Q"},
        {11, "J"},
        {10, "10"},
        {9,  "9"},
        {8,  "8"},
        {7,  "7"},
        {6,  "6"},
        {5,  "5"},
        {4,  "4"},
        {3,  "3"},
        {2,  "2"},
        {1,  "A"}
};

// const string SERVICE_PUBKEY = "EOS67sxYKu379Q9QLb3ERgza8oCN21Z3Rr3FDFii1gzj8iDGifxE7";
//const asset PLAYER_BANKER_MAX_BET_AMOUNT = asset(1000 * 10000, EOS_SYMBOL);
//const asset SYSTEM_BANKER_MAX_BET_AMOUNT = asset(1000 * 10000, EOS_SYMBOL); //系统上庄最大投注额
//const asset CUR_MAX_BET = asset(1000 * 10000, EOS_SYMBOL);                //单人投注最大额


// 枚举 预约上庄action
enum RES_BANK_ACTION {
    up_banker_action = 1,
    down_bank_action,
    increase_banker_action,
    reduce_banker_action,
    cancel_banker_action
};
/**
-1：暂停
1：开始下注
2：停止下注
3：开牌中
4：结算中
5：本局结算完成
 **/
enum GAME_STATE {
    suspended_bet_game_state = -1,
    start_bet_game_state = 1,
    stop_bet_game_state,
    open_card_game_state,
    start_settlement_game_state,
    finish_settlement_game_state,
    cancel_settlement_game_state
};

/*
 * 轮次状态
1：进行中
2：本轮结算中
3：本轮结算完成
 *
 *
 * */
enum ROUND_STATE {
    ongoing_round_state = 1,
    start_settlement_round_state,
    finish_settlement_round_state,
    cancel_settlement_round_state
};

/*
 *             uint32_t state;        // 状态:待结算：1结算中：2已结算：3
 * */
enum BET_STATE {
    pending_settlement_bet_state = 1,
    start_settlement_bet_state,
    //todo::取消逻辑,结算的时候，结束就直接删了不用更新
    finished_settlement_bet_state,
};

/*
 * 1：上庄中
2：预约下庄
3：预约下调，处理成功后，转为1上庄
 * */

enum UP_BANKER_STATE {
    up_banker_state = 1,
    res_down_banker_state,
    res_reduce_banker_state
};

/*
 * 11.牌型枚举值
1:无牛；2:牛1；3:牛2；4:牛3；5:牛4；6:牛5；7:牛6；8:牛7；9:牛8；10:牛9；
11:牛牛；12:顺子牛；13:五花牛；14:同花牛；15:葫芦牛；16:炸弹牛；17:五小牛；18:同花顺；
 NONE（无牛）
BULL 5（牛5）
BULL（牛牛）
FLUSH（同花牛）
FULL HOUSE（葫芦牛）
STRAIGHT（顺子牛）
BOOM BULL（炸弹牛）cc
FULL FACE（五花牛）
SMALL BULL（五小牛）
STRAIGHT FLUSH（同花顺）
 * */

enum CARDS_COMBINATION_TYPE {
    bull_none = 1,
    bull_1, bull_2, bull_3, bull_4, bull_5, bull_6, bull_7, bull_8, bull_9,
    bull_bull, straight_bull, full_face_bull, flush_bull, full_house,
    boom_bull, small_bull, straight_flush
};

/*
 * 10.52张牌数组
[
1，2， 3， 4， 5， 6， 7， 8， 9， 10，11，12，13，//0方块 14 15预留
17，18，19，20，21，22，23，24，25，26，27，28，29，//1梅花 30 31预留
33，34，35，36，37，38，39，40，41，42，43，44，45，//2红桃 46 47预留
49，50，51，52，53，54，55，56，57，58，59，60，61，//3黑桃 62 63预留
]
花色：值 / 16
牌值：值 % 16
 * */

class CARDS_ALLOCATION {
public:
    vector<uint32_t> origin_allocation;   //通过wash_hash 拿到的排序
    vector<uint32_t> max_allocation;     //对自身的120中组合取最大牌型组合
    uint32_t max_allocation_type;       // 最大牌型类型
    string max_allocation_type_name;       // 最大牌型类型对应的名称
    uint32_t max_card_number;           //最大牌型的最大点数
    uint32_t max_card_type;             //最大牌型的最大点数对应的花色
    uint32_t winner_ratio;              // 赢家倍率，1，2，3
    std::string to_json() {
        nlohmann::json cards_allocation_position;
        cards_allocation_position["origin_allocation"] = origin_allocation;
        cards_allocation_position["max_allocation"] = max_allocation;
        cards_allocation_position["max_allocation_type"] = max_allocation_type;
        cards_allocation_position["max_allocation_type_name"] = max_allocation_type_name;
        cards_allocation_position["max_card_number"] = max_card_number;
        cards_allocation_position["max_card_type"] = max_card_type;
        cards_allocation_position["winner_ratio"] = winner_ratio;
        return cards_allocation_position.dump();
    }
};

CARDS_ALLOCATION CARDS_ALLOCATION_DEFAULT = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, "", 0, 0, 0};
//typedef struct cards_allocation CARDS_ALLOCATION;

class CARDS_ALLOCATIONS {
public:
    vector<CARDS_ALLOCATION> data;
    uint32_t data_length;

    std::string to_json() {
        uint32_t i = 0;
        vector<string> tmp_str;
        while (i < data_length) {
            tmp_str.push_back(data[i++].to_json());
        }
        nlohmann::json json_cards_allocations;
        json_cards_allocations["cards_allocation"] = tmp_str;
        return json_cards_allocations.dump();
    }
};

class CARD {
public:
    uint32_t value;
    uint32_t number;
    uint32_t type;
    string type_icon;

    uint32_t to_niu_number() {
        return number > 10 ? 10 : number; //J,Q,K按照10计算
    }
};
