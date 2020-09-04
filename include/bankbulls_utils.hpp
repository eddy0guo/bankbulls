#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/transaction.hpp>
//#include "eosio.token.hpp"
#include <eosio/crypto.hpp>
#include "utils.hpp"

#include <eosio/singleton.hpp>
#include <vector>
#include<algorithm>


using namespace eosio;
using namespace std;

eosio::checksum256 get_current_hash();

int computer_game_result(string users_gestures, string eos_gestures);

void finger_transfer(const name from, const name to, string token_contract_name, int64_t amount, string memo);

string get_eos_gestures();

int32_t get_card_value(vector<uint32_t> all_cards_values, string wash_hash, uint32_t card_number_hex, uint32_t &i);

BET_RESULT compare_allocation(CARDS_ALLOCATION a, CARDS_ALLOCATION b);

ALLOCATION_INFO get_allocation_info(vector<uint32_t> allocation);

CARD get_max_card(vector<CARD> cards);

char sort_allocations(CARDS_ALLOCATIONS &allocations, string block_hash);

void finger_transfer(const name from, const name to, string token_contract_name, int64_t amount, string memo) {
    symbol current_symbol = token_contract_name == "eosio.token" ? EOS_SYMBOL : FINGER_SYMBOL;
    // fixme 判断余额不足
    action(permission_level{from, name("active")},
           name(token_contract_name),
           name("transfer"),
           std::make_tuple(from, to, asset(amount, current_symbol), std::string(memo)))
            .send();
}

eosio::checksum256 get_current_hash() {
    auto size = transaction_size();
    char buf[size];
    uint32_t read = read_transaction(buf, size);
    check(size == read, "read_transaction failed");
    eosio::checksum256 hash = sha256(buf, read);
    print("current hash ", hash);
    return hash;
}
/**

uint32_t random(name name, uint64_t game_id)
{
	asset pool_eos = eosio::token::get_balance(_self, symbol("EOS", 4).code().raw());
	auto mixd = tapos_block_prefix() * tapos_block_num() - current_time() + pool_eos.amount;

	const char *mixedChar = reinterpret_cast<const char *>(&mixd);

	checksum256 result;
	ssha256((char *)mixedChar, sizeof(mixedChar), &result);

	uint64_t random_num = *(uint64_t *)(&result.hash[0]) + *(uint64_t *)(&result.hash[8]) + *(uint64_t *)(&result.hash[16]) + *(uint64_t *)(&result.hash[24]);
	return (uint32_t)(random_num % 3);
}
**/
/**
uint64_t get_account_balance(name account)
{
   uint64_t balance = 0;

   symbol sym(symbol_code("EOS"), 4);
   accounts accountstable("eosio.token"_n, account.value);
   const auto ac = accountstable.find(sym.code().raw());
   check(ac != accountstable.end(), "can not find account!");
   balance = ac->balance.amount / 10000;

   return balance;
}**/

uint64_t get_account_balance(name account) {
    struct accounta {
        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };
    typedef eosio::multi_index<name("accounts"), accounta> accounts;

    uint64_t balance = 0;

    symbol sym(symbol_code("EOS"), 4);
    accounts accountstable(name("eosio.token"), account.value);
    const auto ac = accountstable.find(sym.code().raw());
    check(ac != accountstable.end(), "can not find account!");
    balance = ac->balance.amount;

    return balance;
}

string wash_hash(string seed, string block_hash) {
    string new_seed = seed.append(block_hash.substr(block_hash.length() - 32));
    checksum256 wash_result = eosio::sha256(new_seed.c_str(), new_seed.length());
    //checksum256 new_hash = hex_to_sha256(new_seed);
    return sha256_to_hex(wash_result);
    //return new_hash.to_string();
}
//string wash_result = wash_hash(sha256_to_hex(seed),block_hash); //测试没过
//todo:抽取25张牌，p1,p2,p3,p4,p5每组五张牌
/*
 * 8.抽牌规则如下
1.以wash_HASH字符串，从右向左索引取字符串
2.取牌值：
①取[1-9abcd]对应牌的[A-9，10，J，Q，K]
②不在范围[0ef]的跳过
3.取花色：
①取[0-9-a-b-c-d-e-f]为值m，由[m % 4]决定花色 //fixme:这里是取asicc码还是对应的16进制的字面量值，应该是后者
		0 方块、1梅花、2 红桃、3 黑桃
4.过滤重复牌：
1.取下个牌时，如果前面存在重复的，则花色跳过，重取花色
2.取下个牌时，如果前面已存在4个不同花色且值相同的牌，则值与花色一起跳过，重取牌
3.依次类推
5.最后一位处理：
1.如果最后一位取牌值，没有花色牌，或者 牌相同，重取花色时，则进行第2轮抽牌，进行第6步
6.如果第一轮没有抽满25张牌 或者 第5步发生，则进行第二轮抽牌：
1.将第一轮的HASH值，从索引0 到 第一位有效牌值v，如4f，进行HASH头尾调换重组，如
第一轮：8af11ae1f6903b9608f4f0c12c7d3dc3860707c4bdbc9412c3f5910f5c79024f
第二轮：4f8af11ae1f6903b9608f4f0c12c7d3dc3860707c4bdbc9412c3f5910f5c7902
7.再对第二轮HASH进行1-6步的循环处理，直到抽满25张牌
8.异常处理：进行5轮后，还没抽齐，则跳出，该局取消，退还所有。
 *
 * */

//CARDS_ALLOCATION deal_cards(string wash_result) {
//todo：对8.4 8.5 8.6 8.7的无法开局的清空进行判断处理
CARDS_ALLOCATIONS deal_cards(string wash_result) {
    //wash_result = "55555555555555555551154faf9e33a21f31cc80a97fef4c3bfb7797c05e52f8";
    //wash_result = "5555555555555555555555555555555555555555555555555555555555555555";
    vector<uint32_t> origin_allocation;
    CARDS_ALLOCATION cards_allocation = CARDS_ALLOCATION_DEFAULT;
    CARDS_ALLOCATIONS cards_allocations;
    uint32_t max_round = 5;
    uint32_t end_index = wash_result.length() - 1;
    uint32_t j = 0;

    //all为统计历史已经抽到的牌做反向过滤用
    vector<uint32_t> all_cards_values;
    //下标至少为1
    for (uint32_t i = end_index; i > 0; i--) {
        //char card_number_char = wash_result.substr(i, 1);
        char card_number_char = wash_result.at(i);
        uint32_t card_number_hex = from_hex(card_number_char);
        //printf("   --current i =%d;card_number_char is%c--   ",i,card_number_char);
        if (card_number_char == '0' || card_number_char == 'e' || card_number_char == 'f') {
            continue;
        } else {
            int32_t card_value = get_card_value(all_cards_values, wash_result, card_number_hex, i);
            // hash循环一遍没凑齐25张牌,
            // 1.将第一轮的HASH值，从索引0 到 第一位有效牌值v，如4f，进行HASH头尾调换重组，如
            // hash的第一位落单的时候（要两位才能组成value）也视为结束，从头选
            if (card_value == -2) {
                check(max_round != 0, "deal cards failed3");
                i = end_index - 2 * (5 - --max_round);  //todo:掉头重组
                printf("wash hash again3");
                //8.异常处理：进行5轮后，还没抽齐，则跳出，该局取消，退还所有。
                //todo:先直接报错，后边在补充退还逻辑
                continue;
            }
            if (card_value == -1) {
                printf("   ***i=%d;card_value=%d***   ", i, card_value);
                if (i <= 1) {
                    check(max_round != 0, "deal cards failed2");
                    i = end_index - 2 * (5 - --max_round);  //todo:掉头重组
                    printf("wash hash again2");
                }
                continue;
            }

            origin_allocation.push_back(card_value);
            all_cards_values.push_back(card_value);
            printf("  $$$$$%d=%d    ", origin_allocation.size(), all_cards_values.size());
            //选好了一组牌
            if (origin_allocation.size() == 5) {
                cards_allocation.origin_allocation = origin_allocation;
                cards_allocations.data.push_back(cards_allocation);
                //选好了五组牌
                if (cards_allocations.data.size() == 5) {
                    break;
                }
                cards_allocation = CARDS_ALLOCATION_DEFAULT;
                origin_allocation.clear();
            }
            if (i <= 1) {
                check(max_round != 0, "deal cards failed1");
                i = end_index - 2 * (5 - --max_round);  //todo:掉头重组
                printf("wash hash again1");
            }
        }
    }
    cards_allocations.data_length = cards_allocations.data.size();
    printf("length=%d ", cards_allocations.data_length);
    check(cards_allocations.data_length == 5, "deal_cards failed");
    return cards_allocations;
}

//todo::计算每组手牌的最大的牌型
void get_max_combination(CARDS_ALLOCATIONS &cards_allocations) {
    uint32_t length = cards_allocations.data.size(); //=5
    // uint32_t length = 5;
    //vector<CARDS_ALLOCATION> kkkk;
    while (length--) {
        //开牌初始开拍顺序作为初始max组合
        vector<uint32_t> origin_allocation = cards_allocations.data[length].origin_allocation;
        ALLOCATION_INFO info = get_allocation_info(cards_allocations.data[length].origin_allocation);
        cards_allocations.data[length].max_allocation = origin_allocation;
        cards_allocations.data[length].max_allocation_type = std::get<0>(info);
        cards_allocations.data[length].max_allocation_type_name = std::get<1>(info);
        cards_allocations.data[length].max_card_number = std::get<2>(info);
        cards_allocations.data[length].max_card_type = std::get<3>(info);
        cards_allocations.data[length].winner_ratio = std::get<4>(info);
        CARDS_ALLOCATION tmp_max_allocation = cards_allocations.data[length];

        uint32_t tmp_max_card_number = 0;
        vector<uint32_t> cards = {0, 0, 0, 0, 0};
        /**
        printf("   origin=[%d,%d,%d,%d,%d]  ", origin_allocation[0],
               origin_allocation[1], origin_allocation[2],
               origin_allocation[3], origin_allocation[4]);
               **/
        for (int i = 0; i < 5; ++i) {
            cards[0] = origin_allocation[i];
            for (int j = 0; j < 5; ++j) {
                if (j == i) continue;
                cards[1] = origin_allocation[j];
                for (int k = 0; k < 5; ++k) {
                    if (k == j || k == i) continue;
                    cards[2] = origin_allocation[k];
                    for (int l = 0; l < 5; ++l) {
                        if (l == k || l == j || l == i) continue;
                        cards[3] = origin_allocation[l];
                        for (int m = 0; m < 5; ++m) {
                            if (m == l || m == k || m == j || m == i) continue;
                            cards[4] = origin_allocation[m];

                            ALLOCATION_INFO tmp_info = get_allocation_info(cards);
                            CARDS_ALLOCATION tmp_current_allocation = {
                                    origin_allocation,
                                    cards,
                                    std::get<0>(tmp_info),
                                    std::get<1>(tmp_info),
                                    std::get<2>(tmp_info),
                                    std::get<3>(tmp_info),
                                    std::get<4>(tmp_info),
                            };
                            BET_RESULT result = compare_allocation(tmp_max_allocation, tmp_current_allocation);
                            // printf("   resultt=%d    ",result);
                            if (std::get<0>(result) > 0) {
                                tmp_max_allocation = tmp_current_allocation;
                            }
                            /**
                            if(result != 0){
                                printf("   tmp_current_allocation=%s;",tmp_current_allocation.to_json().data());
                                printf(";tmp_max_allocation=%s    ",tmp_max_allocation.to_json().data());
                            }**/
                            //todo:无牛的情况下倒vector cards
                        }
                    }
                }
            }
        }
        cards_allocations.data[length] = tmp_max_allocation;
        //printf("  length=%d  ", kkkk.size());
    }
}

//todo::计算庄和手牌顺序
//string[] new_cards_allocation = sort_cards(wash_result);
//todo::输赢是牌型大小比较,-1,输，1赢，0平（自我比较求max牌型的时候同花顺，炸弹牛可能出现平局，庄和闲比较的时候不会和）
//输赢是针对b来说的
BET_RESULT compare_allocation(CARDS_ALLOCATION a, CARDS_ALLOCATION b) {
    BET_RESULT result = std::make_tuple(0, "");
    BET_RESULT win = std::make_tuple(1, "WIN");
    BET_RESULT lose = std::make_tuple(-1, "LOSE");
    BET_RESULT tied = std::make_tuple(0, "TIED");
    //ALLOCATION_INFO info = std::make_tuple(0, "", max_card.number, max_card.type, 0);

    if (a.max_allocation_type == b.max_allocation_type) {
        if (a.max_card_number == b.max_card_number) {
            //牌型，最大点数，花色都一样
            if (a.max_card_type == b.max_card_type) {
                result = tied;
            } else {
                // 黑红梅方，3，2，1，0，值大的大
                result = a.max_card_type > b.max_card_type ? lose : win;
            }
        } else {
            //printf(" a.max_card_number=%d,b.max_card_number=%d ",a.max_card_number,b.max_card_number);
            result = a.max_card_number > b.max_card_number ? lose : win;
        }
    } else {
        //printf(" a.max_allocation_type=%d,b.max_allocation_type=%d ",a.max_allocation_type,b.max_allocation_type);
        result = a.max_allocation_type > b.max_allocation_type ? lose : win;
    }
    return result;
}

/**1:无牛；2:牛1；3:牛2；4:牛3；5:牛4；6:牛5；7:牛6；8:牛7；9:牛8；10:牛9；
11:牛牛；12:顺子牛；13:五花牛；14:同花牛；15:葫芦牛；16:炸弹牛；17:五小牛；18:同花顺；
 enum cards_combination_type {
    bull_none = 1,
    bull_1, bull_2, bull_3, bull_4, bull_5, bull_6, bull_7, bull_8, bull_9,
    bull, straight_bull, full_face_bull, flush_bull, full_house,
    boom_bull, small_bull, straight_flush
};
 * **/
ALLOCATION_INFO get_allocation_info(vector<uint32_t> allocation) {
    vector<CARD> new_allocation = {};
    for (auto iter = allocation.cbegin(); iter != allocation.cend(); iter++) {
        CARD tmp_card = {*iter, *iter % 16, *iter / 16};
        new_allocation.push_back(tmp_card);
    }
    CARD max_card = get_max_card(new_allocation);
    //组合,组合名称,最大点数，最大点数对应的花色，赔率，
    ALLOCATION_INFO info = std::make_tuple(0, "", max_card.number, max_card.type, 0);
    if ((new_allocation[0].value - new_allocation[1].value == 1
         && new_allocation[0].value - new_allocation[2].value == 2
         && new_allocation[0].value - new_allocation[3].value == 3
         && new_allocation[0].value - new_allocation[4].value == 4)
        || (new_allocation[0].value - new_allocation[1].value == -1
            && new_allocation[0].value - new_allocation[2].value == -2
            && new_allocation[0].value - new_allocation[3].value == -3
            && new_allocation[0].value - new_allocation[4].value == -4)
            ) {
        std::get<0>(info) = straight_flush;
        std::get<1>(info) = "straight_flush";
        std::get<4>(info) = 3;
    } else if (new_allocation[0].number < 5
               && new_allocation[1].number < 5
               && new_allocation[2].number < 5
               && new_allocation[3].number < 5
               && new_allocation[4].number < 5
               && new_allocation[0].number + new_allocation[1].number + new_allocation[2].number +
                  new_allocation[3].number + new_allocation[4].number <= 10
            ) {
        //type_rate = std::make_tuple(small_bull,3);
        std::get<0>(info) = small_bull;
        std::get<1>(info) = "small_bull";
        std::get<4>(info) = 3;
    } else if (new_allocation[0].number == new_allocation[1].number
               && new_allocation[0].number == new_allocation[2].number
               && new_allocation[0].number == new_allocation[3].number
            ) {
        //type_rate = std::make_tuple(boom_bull,3);
        std::get<0>(info) = boom_bull;
        std::get<1>(info) = "boom_bull";
        std::get<4>(info) = 3;
    } else if (
            new_allocation[0].number == new_allocation[1].number
            && new_allocation[0].number == new_allocation[2].number
            && new_allocation[0].number == new_allocation[4].number
            ) {
        //type_rate = std::make_tuple(full_house,3);
        std::get<0>(info) = full_house;
        std::get<1>(info) = "full_house";
        std::get<4>(info) = 3;
    } else if (new_allocation[0].type == new_allocation[1].type
               && new_allocation[0].type == new_allocation[2].type
               && new_allocation[0].type == new_allocation[3].type
               && new_allocation[0].type == new_allocation[4].type
            ) {
        // type_rate = std::make_tuple(flush_bull,3);
        std::get<0>(info) = flush_bull;
        std::get<1>(info) = "flush_bull";
        std::get<4>(info) = 3;
    } else if (new_allocation[0].number > 10
               && new_allocation[1].number > 10
               && new_allocation[2].number > 10
               && new_allocation[3].number > 10
               && new_allocation[4].number > 10
            ) {
        //type_rate = std::make_tuple(full_face_bull,3);
        std::get<0>(info) = full_face_bull;
        std::get<1>(info) = "full_face_bull";
        std::get<4>(info) = 3;
    } else if ((new_allocation[0].number - new_allocation[1].number == 1
                && new_allocation[0].number - new_allocation[2].number == 2
                && new_allocation[0].number - new_allocation[3].number == 3
                && new_allocation[0].number - new_allocation[4].number == 4)
               || (new_allocation[0].number - new_allocation[1].number == -1
                   && new_allocation[0].number - new_allocation[2].number == -2
                   && new_allocation[0].number - new_allocation[3].number == -3
                   && new_allocation[0].number - new_allocation[4].number == -4)) {
        //type_rate = std::make_tuple(straight_bull,3);
        std::get<0>(info) = straight_bull;
        std::get<1>(info) = "straight_bull";
        std::get<4>(info) = 3;
    } else if ((new_allocation[0].to_niu_number() + new_allocation[1].to_niu_number() +
                new_allocation[2].to_niu_number()) % 10 == 0
               && (new_allocation[3].to_niu_number() + new_allocation[4].to_niu_number()) % 10 == 0
            ) {
        //type = bull;
        std::get<0>(info) = bull_bull;
        std::get<1>(info) = "bull_bull";
        std::get<4>(info) = 3;
    } else if (
            (new_allocation[0].to_niu_number() + new_allocation[1].to_niu_number() +
             new_allocation[2].to_niu_number()) % 10 == 0
            && (new_allocation[3].to_niu_number() + new_allocation[4].to_niu_number()) % 10 > 0
            ) {
        uint32_t remainder = (new_allocation[3].to_niu_number() + new_allocation[4].to_niu_number()) % 10;
        uint32_t bull_type = bull_none + remainder;
        uint32_t rate = bull_type >= 8 ? 2 : 1;

        string bull_type_name = "bull_";
        bull_type_name.append(uint_string(bull_type - 1));
        std::get<0>(info) = bull_type;
        std::get<1>(info) = bull_type_name;
        std::get<4>(info) = rate;
    } else if ((new_allocation[0].to_niu_number() + new_allocation[1].to_niu_number() +
                new_allocation[2].to_niu_number()) % 10 != 0) {
        std::get<0>(info) = bull_none;
        std::get<1>(info) = "bull_none";
        std::get<4>(info) = 1;
    } else {
        //todo：不允许存在的情况，调试先直接崩掉，生产printf
        check(false, "unknown cards allocation type");
    }
    /**
    printf("  allocation=[%d,%d,%d,%d,%d];type=%d   ",allocation[0],
            allocation[1],allocation[2],allocation[3],allocation[4],std::get<0>(info));
            **/
    return info;
}


CARD get_max_card(vector<CARD> cards) {
    CARD max_card = {0, 0, 0};
    for (auto iter = cards.cbegin(); iter != cards.cend(); iter++) {
        if (iter->number == max_card.number) {
            // 黑红梅方，3，2，1，0，值小的大,且type+number唯一
            max_card = iter->type > max_card.type ? *iter : max_card;
        } else {
            max_card = iter->number > max_card.number ? *iter : max_card;
        }
    }
    return max_card;
}

int32_t get_card_value(vector<uint32_t> all_cards_values, string wash_hash, uint32_t card_number_hex, uint32_t &i) {
    //fixme: 改为enum
    int32_t card_value = -2;
    uint32_t j = 0;
    while (i--) {
        char card_type_char = wash_hash.at(i);
        //  printf("   ++current i =%d;card_type_char is%c++   ",i,card_type_char);
        uint32_t card_type_hex = from_hex(card_type_char);

        uint32_t card_type = card_type_hex % 4;
        int32_t tmp_card_value = card_number_hex + card_type * 16;

        //printf(" --%x-%x--  ",card_number_hex,card_type_hex);
        //1.取下个牌时，如果前面存在重复的，则花色跳过，重取花色
        // 2.取下个牌时，如果前面已存在4个不同花色且值相同的牌，则值与花色一起跳过，重取牌
        vector<uint32_t>::iterator it = find(all_cards_values.begin(), all_cards_values.end(), tmp_card_value);
        if (it != all_cards_values.end()) {
            //printf("  i=%d,card_value=%d   ", i, card_value);
            if (j++ >= 3) {
                return -1; //fixme:这里-1只是作为四色卡牌已经齐了
            }
            continue;
        } else {
            card_value = tmp_card_value;
            break;
        }
    }
    //printf("  ++i2=%d,card_value=%d++   ", i, card_value);
    return card_value;
}

//根据block_hash排序
char sort_allocations(CARDS_ALLOCATIONS &allocations, string block_hash) {
    uint32_t length = block_hash.length();//64
    char last_char;
    while (--length) {
        last_char = block_hash.at(length);
        if (last_char != 'f') break;
    }
    check(length != 0, "illegal block_hash");
    uint32_t last_char_hex = from_hex(last_char);
    printf("last_char_hex=%x", last_char_hex);
    uint32_t banker_index = last_char_hex % 5;
    vector<CARDS_ALLOCATION> tmp_allocations;
    //uint32_t banker_index =
    //allocations.data_length = 5
    for (uint32_t i = banker_index; i < 5; i++) {
        tmp_allocations.push_back(allocations.data[i]);
        //走到末尾还没填满vector,从0(-1 + 1 = 0)开始
        if (tmp_allocations.size() != 5 && i == 4) {
            i = -1;
        }
        if (tmp_allocations.size() == 5) break;
    }
    allocations.data = tmp_allocations;
    return last_char;
}

vector<string> flush_cards_allocations_to_table(name finger_official, CARDS_ALLOCATIONS cards_allocations,
                                                string wash_result, string block_hash, char sort_char) {
    //庄家牌序号
    uint32_t length = block_hash.length();//64
    char last_char;
    while (--length) {
        last_char = block_hash.at(length);
        if (last_char != 'f') break;
    }
    check(length != 0, "illegal block_hash");
    uint32_t last_char_hex = from_hex(last_char);

    uint64_t last_game_id = models::game::get_last_id(finger_official);
    vector<string> allocation_infos;
    vector<string> tmp_card_jsons;
    vector<int32_t> bet_results;
    //下标0为庄
    for (uint32_t i = 0; i < 5; i++) {
        string tmp_allocation_info = "";
        string tmp_card_json = "";
        tmp_card_json.clear();
        tmp_allocation_info.clear();

        BET_RESULT result = compare_allocation(cards_allocations.data[0], cards_allocations.data[i]);
        for (uint32_t j = 0; j < 5; j++) {
            uint32_t card_value = cards_allocations.data[i].max_allocation[j];
            uint32_t card_number = card_value % 16;
            uint32_t card_type = card_value / 16;
            string card_number_icon = CARD_NUMBER_ICON.find(card_number)->second;
            string card_type_icon = CARD_TYPE_ICON.find(card_type)->second;
            //std::sprintf(tmp_info, "%d[%s%s],", card_value, card_type_icon.data(), card_number_icon.data());
            tmp_allocation_info += uint_string(card_value) + "[" + card_type_icon + card_number_icon + "]";
            tmp_card_json += uint_string(card_value) + "|";
        }

        //通过当前的排序和hash最后排序的char来反推原始选牌顺序
        uint32_t last_char_hex = from_hex(last_char);
        uint64_t allocation_index = i + 1 + last_char_hex % 5;
        if (allocation_index > 5) {
            allocation_index -= 5;
        }
        //庄家不拼接输赢结果
        string user_bet_result_str;
        if (i == 0) {
            tmp_allocation_info += cards_allocations.data[i].max_allocation_type_name;
            user_bet_result_str = "0";
        } else {
            tmp_allocation_info += cards_allocations.data[i].max_allocation_type_name + "," + std::get<1>(result);
            int32_t user_bet_result = std::get<0>(result) == 1 ? cards_allocations.data[i].winner_ratio
                                                               : -1 * cards_allocations.data[0].winner_ratio;
            user_bet_result_str =
                    user_bet_result > 0 ? uint_string(user_bet_result) : "-" + uint_string(-user_bet_result);
            bet_results.push_back(user_bet_result);
        }
        tmp_card_json += uint_string(allocation_index) + "|" +
                         uint_string(cards_allocations.data[i].max_allocation_type) + "|" + user_bet_result_str;

        //printf("  $$$$$%s$$$$$  ", tmp_info);
        allocation_infos.push_back(tmp_allocation_info);
        //printf("   4[%s   ",tmp_card_json.data());
        tmp_card_jsons.push_back(tmp_card_json);
    }
    checksum256 current_txid = get_current_hash();
    string txid_str = sha256_to_hex(current_txid);
    models::game::update(finger_official, last_game_id, allocation_infos, bet_results, last_char_hex, wash_result,
                         txid_str);
    return tmp_card_jsons;
}

void flush_cards_allocations_to_block(name contract) {
    uint64_t last_game_id = models::game::get_last_id(contract);
    bankbulls::game game = models::game::find(contract, last_game_id);

    std::tuple<uint64_t, uint32_t, uint32_t, string, string, string, string, string, int32_t,
            int32_t, int32_t, int32_t, uint32_t, uint32_t, uint64_t, string, string, string, string>
            params = std::make_tuple(game.id, game.state, game.dealer_hand_number,
                                     game.dealer_card, game.spades_card, game.hearts_card,
                                     game.clubs_card, game.diamonds_card, game.spades_result,
                                     game.hearts_result, game.clubs_result, game.diamonds_result,
                                     game.reveal_at, game.created_at, game.block_index, game.block_hash,
                                     game.seed, game.sign, game.wash_hash);
    action(permission_level{contract, name("active")},
           contract,
           name("record"),
           params)
            .send();
}

void start_settlement_game(name contract, uint32_t dispose_count, uint32_t game_id) {
    vector<bankbulls::game> games = models::game::list_by_created(contract);
    bankbulls::game last_game = games[0];
    //uint64_t last_game_id = games[0].id;
    uint64_t last_game_id = game_id;


    uint64_t last_cur_round_id = models::cur_round::get_last_id(contract);
    vector<bankbulls::bets> bets_list = models::bets::list(contract, last_game_id, start_settlement_bet_state);
    //check(dispose_count <= bets_list.size(), "dispose_count must be less than bets length");
    dispose_count = dispose_count <= bets_list.size() ? dispose_count : bets_list.size();
    printf("-----%d--", bets_list[0].created_at);


    //玩家的输赢和庄家的相反
    int64_t current_banker_win_amount = 0;
    int64_t current_banker_lose_amount = 0;
    int64_t banker_win_lose_amount = 0;
    vector<string> winner_infos;
    for (uint32_t i = 0; i < dispose_count; i++) {
        int64_t payout_spades_amount = bets_list[i].spades_amt.amount * last_game.spades_result;
        int64_t payout_hearts_amount = bets_list[i].hearts_amt.amount * last_game.hearts_result;
        int64_t payout_clubs_amount = bets_list[i].clubs_amt.amount * last_game.clubs_result;
        int64_t payout_diamonds_amount = bets_list[i].diamonds_amt.amount * last_game.diamonds_result;

        //fee向下取整
        if (payout_spades_amount > 0) {
            payout_spades_amount = payout_spades_amount * 985 / 1000 + 1;
            current_banker_lose_amount += payout_spades_amount;
        } else {
            current_banker_win_amount += payout_spades_amount * -1;
        }
        asset payout_spades = {payout_spades_amount, bets_list[i].spades_amt.symbol};

        if (payout_hearts_amount > 0) {
            payout_hearts_amount = payout_hearts_amount * 985 / 1000 + 1;
            current_banker_lose_amount += payout_hearts_amount;
        } else {
            current_banker_win_amount += payout_hearts_amount * -1;
        }
        asset payout_hearts = {payout_hearts_amount, bets_list[i].hearts_amt.symbol};

        if (payout_clubs_amount > 0) {
            payout_clubs_amount = payout_clubs_amount * 985 / 1000 + 1;
            current_banker_lose_amount += payout_clubs_amount;
        } else {
            current_banker_win_amount += payout_clubs_amount * -1;
        }
        asset payout_clubs = {payout_clubs_amount, bets_list[i].clubs_amt.symbol};

        if (payout_diamonds_amount > 0) {
            payout_diamonds_amount = payout_diamonds_amount * 985 / 1000 + 1;
            current_banker_lose_amount += payout_diamonds_amount;
        } else {
            current_banker_win_amount += payout_diamonds_amount * -1;
        }

        asset payout_diamonds = {payout_diamonds_amount, bets_list[i].diamonds_amt.symbol};
        //庄家输赢和闲家输赢相反
        banker_win_lose_amount += payout_spades_amount * -1 + payout_hearts_amount * -1 +
                                  payout_clubs_amount * -1 + payout_diamonds_amount * -1;
        if (banker_win_lose_amount < 0) {
            string winner_info = player_winners_json(bets_list[i].player, asset(-banker_win_lose_amount, EOS_SYMBOL));
            winner_infos.push_back(winner_info);
        }

        //逐笔打款
        int64_t payout_all = bets_list[i].bet_amount.amount * 3 +
                             payout_spades_amount +
                             payout_hearts_amount +
                             payout_clubs_amount +
                             payout_diamonds_amount;
        printf(" --payout_all=%d--  ", payout_all);
        uint128_t sender_id = TRANSFER_SID + last_game_id * 100 + i;
        if (games[0].state == cancel_settlement_game_state) {
            payout_all = bets_list[i].bet_amount.amount * 3;
            //100为并行结算的时候，两批game-id的隔离区间，防止并行结算senderid冲突
            printf("  1a ");
            delay_call_action(name("eosio.token"),
                              contract,
                              name("transfer"),
                              std::make_tuple(contract, bets_list[i].player, asset(payout_all, EOS_SYMBOL),
                                              std::string("cancel bank bull settlement")),
                              1, sender_id);
            printf(" 1b ");
            //fixme:不更新直接删？
            //models::bets::update(contract, bets_list[i].id, finished_settlement_bet_state);
            printf("   yy1-%d   ",bets_list[i].id);
            models::bets::remove(contract, bets_list[i].id);
            continue;
        }
        //finger_transfer(contract, bets_list[i].player, "eosio.token", payout_all, "bank bull settlement");
        delay_call_action(name("eosio.token"),
                          contract,
                          name("transfer"),
                          std::make_tuple(contract, bets_list[i].player, asset(payout_all, EOS_SYMBOL),
                                          std::string("bank bull settlement")),
                          1, sender_id);
        //models::bets::update(contract, bets_list[i].id, finished_settlement_bet_state);
        models::bets::remove(contract, bets_list[i].id);


        //block存证
        std::tuple<uint64_t, name, uint32_t,
                string, string, string,
                string, string, asset,
                asset, asset, asset, asset,
                asset, asset, asset, asset,
                uint32_t, string, string,
                string, uint64_t, string> params = std::make_tuple(
                last_game.id, bets_list[i].player, last_game.dealer_hand_number,
                last_game.dealer_card, last_game.spades_card, last_game.hearts_card,
                last_game.clubs_card, last_game.diamonds_card, bets_list[i].spades_amt,
                bets_list[i].hearts_amt, bets_list[i].clubs_amt, bets_list[i].diamonds_amt, bets_list[i].bet_amount,
                payout_spades, payout_hearts, payout_clubs, payout_diamonds,
                last_game.reveal_at, last_game.seed, last_game.sign,
                last_game.public_key, last_game.block_index, last_game.block_hash);
        //todo：调用eos合约打钱，保证金+payout就是最终的用户钱
        //todo：如果参数相同则只内联一次,如果问题无法解决则要引入betsid参数来保证每次调用参数的唯一
        action(permission_level{contract, name("active")},
               contract,
               name("reveal"),
               params)
                .send();
    }
    //流局的话就不在更新相关表数据和日志
    if (games[0].state == cancel_settlement_game_state) {
        return;
    }
    //更新轮次累积输赢
    models::cur_round::update(contract, last_cur_round_id, current_banker_win_amount,
                              current_banker_lose_amount, banker_win_lose_amount);
    //根据是否还有待结算的下注，来判断要更新的状态为进行中还是结算已完成
    bets_list = models::bets::list(contract, last_game_id, start_settlement_bet_state);
    if (bets_list.size() == 0) {
        models::game::update(contract, last_game_id, finish_settlement_game_state);
        string game_settlement_info = game_settlement_json(last_game_id, finish_settlement_game_state,
                                                           asset(current_banker_lose_amount, EOS_SYMBOL), winner_infos);
        //logs2为结算完成表
        models::logs2::insert(contract, game_settlement_info);
    } else {
        models::game::update(contract, last_game_id, start_settlement_game_state);
        string game_settlement_info = game_settlement_json(last_game_id, start_settlement_game_state,
                                                           asset(current_banker_lose_amount, EOS_SYMBOL), winner_infos);
        models::logs::insert(contract, game_settlement_info);
    }
    return;
}

bool update_up_banker_and_settlement(name contract) {
    printf(" *a* ");
    // todo: 预约表数据转移到上庄表，&&清理预约表，没有预约上下庄的，自动
    vector<bankbulls::up_bankers> up_banker_list = models::up_bankers::list(contract);
    vector<bankbulls::res_bankers> res_banker_list = models::res_bankers::list(contract);

    //只更新投资额
    printf(" *b* ");
    for (uint32_t i = 0; i != res_banker_list.size(); i++) {
        if (res_banker_list[i].action == up_banker_action) {
            models::up_bankers::insert(contract, res_banker_list[i].account, res_banker_list[i].investment,
                                       asset(0, EOS_SYMBOL), asset(0, EOS_SYMBOL), 0, 0, 0,
                                       0, up_banker_state, res_banker_list[i].investment);
            continue;
        }
        for (uint32_t j = 0; j != up_banker_list.size(); j++) {
            if (up_banker_list[j].account == res_banker_list[i].account) {
                switch (res_banker_list[i].action) {
                    case down_bank_action : {
                        //删除next中对应的原来的庄
                        //models::up_bankers::remove(contract, res_banker_list[i].account);
                        //int64_t payout_all = up_banker_list[j].investment.amount;
                        //finger_transfer(contract, up_banker_list[j].account, "eosio.token",
                        //              payout_all, "banker bulls down bank settlement");
                        models::up_bankers::update(contract, res_banker_list[i].account,
                                                   up_banker_list[j].investment, res_down_banker_state,
                                                   up_banker_list[j].investment);
                        break;
                    }
                    case increase_banker_action: {
                        //up_item.investment.amount += res_item.investment.amount;
                        int64_t next_investment_amount = up_banker_list[j].investment.amount +
                                                         res_banker_list[i].investment.amount;
                        asset next_investment = asset(next_investment_amount, EOS_SYMBOL);
                        //上调的状态也为up_banker_state
                        models::up_bankers::update(contract, res_banker_list[i].account,
                                                   next_investment, up_banker_state, res_banker_list[i].investment);
                        break;
                    }
                    case reduce_banker_action: {
                        int64_t next_investment_amount = up_banker_list[j].investment.amount -
                                                         res_banker_list[i].investment.amount;
                        //透支下调不在报错，改为直接下庄
                        //check(next_investment_amount > 0, "next investment amount must be more than zero");
                        asset next_investment = asset(next_investment_amount, EOS_SYMBOL);
                        //减仓生效后若余额低于50EOS将自动下庄。
                        bankbulls::global_state limit_system_bank = models::global_state::find(contract,
                                                                                               name(LIMIT_SYSTEM_BANK_KEY_NAME));
                        uint64_t limit_system_bank_amount = strtoull(limit_system_bank.value.c_str(), NULL, 0);

                        if (next_investment_amount >= limit_system_bank_amount) {
                            models::up_bankers::update(contract, res_banker_list[i].account,
                                                       next_investment, res_reduce_banker_state,
                                                       res_banker_list[i].investment);
                        } else {
                            next_investment.amount =
                                    up_banker_list[j].investment.amount;
                            models::up_bankers::update(contract, res_banker_list[i].account,
                                                       next_investment, res_down_banker_state,
                                                       res_banker_list[i].investment);
                        }

                        //finger_transfer(contract, up_banker_list[j].account, "eosio.token",
                        //              res_banker_list[i].investment.amount, "banker bulls down bank settlement");
                        break;
                    }
                    default:
                        check(false, "res_banker action unknown");
                        break;
                }
            }
        }
    }


    //计算上庄总金额,并把其中的系统庄删除
    printf(" *c* ");
    uint64_t sum_investment = 0;
    up_banker_list = models::up_bankers::list(contract);
    for (uint32_t k = 0; k != up_banker_list.size(); k++) {
        if (up_banker_list[k].state == res_down_banker_state) {
            continue;
        }
        if (up_banker_list[k].account == contract) {
            models::up_bankers::remove(contract, contract);
            continue;
        }
        sum_investment += up_banker_list[k].investment.amount;
    }
    printf(" *d* ");
    //小于阀值，系统以所有金额补庄
    bankbulls::global_state limit_system_bank = models::global_state::find(contract, name(LIMIT_SYSTEM_BANK_KEY_NAME));
    uint64_t limit_system_bank_amount = strtoull(limit_system_bank.value.c_str(), NULL, 0);
    if (sum_investment < 20 * limit_system_bank_amount) {
        asset balance = asset(get_account_balance(contract), EOS_SYMBOL);
        models::up_bankers::insert(contract, contract, balance, balance, asset(0, EOS_SYMBOL), 0, 0, 0, 0, 1,
                                   DEFAULT_ASSET);
        sum_investment += balance.amount;
    }

    printf(" *e* ");
    //更新投资占比，余额，轮次id等
    up_banker_list = models::up_bankers::list(contract);
    for (uint32_t m = 0; m != up_banker_list.size(); m++) {
        asset balance = asset(get_account_balance(up_banker_list[m].account), EOS_SYMBOL);
        name user = up_banker_list[m].account;
        //fixme:要不要考虑取整的问题，比例之和不等于1
        //u64最大922337203-6854775808*2
        uint64_t bet_ratio = up_banker_list[m].investment.amount * 1000000 / sum_investment;
        asset total_divide = asset(0, EOS_SYMBOL);
        uint32_t created_at = current_time_point().sec_since_epoch();
        uint64_t start_game_id = models::game::get_last_id(contract) + 1;
        uint64_t start_round_id = models::cur_round::get_last_id(contract) + 1;
        models::up_bankers::update(contract, user, balance, total_divide,
                                   created_at, bet_ratio, start_game_id, start_round_id);
    }


    return true;
}

void update_cur_round(name contract, uint64_t last_round_id) {
    asset zero_eos = asset(0, EOS_SYMBOL);
    uint32_t created_at = current_time_point().sec_since_epoch();
    uint32_t stop_at = created_at + 12 * 60;
    uint64_t last_game_id = models::game::get_last_id(contract);

    asset player_investment = zero_eos;
    asset system_investment = zero_eos;
    asset player_banker_balance = zero_eos;
    asset system_banker_balance = zero_eos;
    system_banker_balance.amount = get_account_balance(contract);
    uint32_t banker_num = 0;
    vector<bankbulls::up_bankers> up_banker_list = models::up_bankers::list(contract);
    for (uint32_t k = 0; k != up_banker_list.size(); k++) {
        //待下庄的数据不在统计
        if (up_banker_list[k].state == res_down_banker_state) {
            continue;
        }
        banker_num++;
        if (up_banker_list[k].account == contract) {
            system_investment.amount = up_banker_list[k].investment.amount;
            //system_banker_balance.amount = up_banker_list[k].balance.amount;
            continue;
        }
        player_investment.amount += up_banker_list[k].investment.amount;
        player_banker_balance.amount += up_banker_list[k].balance.amount;
    }
    /*
     * //player_banker_max_bet_amount	asset	玩家上庄最大投注总额
//        system_banker_max_bet_amount	asset	仅系统上庄最大投注总额
//cur_max_bet	asset	单人投注最大额
     * */

    asset player_banker_max_bet_asset = player_investment;
    player_banker_max_bet_asset.amount = player_investment.amount * 15 / 1000;
    asset system_banker_max_bet_asset = system_investment;
    system_banker_max_bet_asset.amount = system_investment.amount * 15 / 1000;

    string cur_max_limit = player_investment.amount == 0 ? LIMIT_SYSTEM_BANK_KEY_NAME : LIMIT_PLAY_BANK_KEY_NAME;
    bankbulls::global_state limit_setting = models::global_state::find(contract, name(cur_max_limit));
    uint64_t limit_setting_amount = strtoull(limit_setting.value.c_str(), NULL, 0);
    asset cur_max_asset = asset(limit_setting_amount, EOS_SYMBOL);

    models::cur_round::insert(contract, last_round_id + 1, last_game_id, player_investment,
                              system_investment, player_banker_balance, system_banker_balance,
                              player_banker_max_bet_asset, system_banker_max_bet_asset, cur_max_asset,
                              banker_num, zero_eos, zero_eos, zero_eos, zero_eos,
                              zero_eos, zero_eos, zero_eos,
                              zero_eos, ongoing_round_state, created_at, stop_at);
    return;
}

void insert_hisround_from_curound(name contract, bankbulls::cur_round cur_round) {
    uint64_t id = cur_round.id;
    vector<bankbulls::his_rounds> last_his_round = models::his_round::get_last(contract);
    uint64_t start_game_id = last_his_round.size() == 0 ? 1 : last_his_round[0].end_game_id;
    uint64_t end_game_id = cur_round.cur_game_id;
    asset bank_investment = asset(cur_round.player_investment.amount +
                                  cur_round.system_investment.amount, EOS_SYMBOL);
    asset bank_balance = asset(cur_round.player_banker_balance.amount +
                               cur_round.system_banker_balance.amount, EOS_SYMBOL);
    asset bank_profit = cur_round.banker_win_lose;
    asset sys_investment = cur_round.system_investment;
    asset sys_balance = cur_round.system_banker_balance;
    //系统总盈利=输赢总额 减掉用户庄分成的余下的额度
    vector<bankbulls::up_bankers> up_bankers_list = models::up_bankers::list(contract);
    int64_t sum_bankers_divide = 0;
    for (uint32_t i = 0; i < up_bankers_list.size(); i++) {
        if (up_bankers_list[i].account == contract) {
            continue;
        }
        sum_bankers_divide += up_bankers_list[i].total_divide.amount;
    }
    asset sys_profit = asset(cur_round.banker_win_lose.amount - sum_bankers_divide, EOS_SYMBOL);
    uint64_t created_at = cur_round.created_at;
    uint64_t end_time = current_time_point().sec_since_epoch();
    models::his_round::insert(contract, id, start_game_id, end_game_id,
                              bank_investment, bank_balance, bank_profit,
                              sys_investment, sys_balance, sys_profit,
                              created_at, end_time);
    return;
}

/*插入bankersup
 * 更新curround---比例，投资额
 * */
void system_join_bankers(name contract) {
    vector<bankbulls::cur_round> cur_round = models::cur_round::list_by_created(contract);
    //fixme::这里是否要过滤状态，此时游戏结算的时候已经都是状态1了
    vector<bankbulls::up_bankers> up_bankers = models::up_bankers::list(contract);
    int64_t sys_balance_amt = get_account_balance(contract);
    int64_t total_investment_amt = sys_balance_amt + cur_round[0].player_investment.amount;
    asset sys_balance_asset = asset(sys_balance_amt, EOS_SYMBOL);
    uint32_t created_at = current_time_point().sec_since_epoch();
    uint64_t sum_sys_ratio = 0;
    for (auto item:up_bankers) {

        uint64_t bet_ratio = item.investment.amount * 1000000 / total_investment_amt;
        sum_sys_ratio += item.investment.amount * 1000000 / total_investment_amt;
        models::up_bankers::update(contract, item.account, bet_ratio, "test1", "test2");
    }
    /**
     * static bool insert(name contract, name user, asset investment, asset balance, asset total_divide,
                           uint32_t created_at, uint64_t bet_ratio, uint64_t start_game_id,
                           uint64_t start_round_id, uint32_t state, asset res_amt) {
     * **/
    uint64_t sys_bets_ratio = 1000000 - sum_sys_ratio;
    models::up_bankers::insert(contract, contract, sys_balance_asset, sys_balance_asset, DEFAULT_ASSET, created_at,
                               sys_bets_ratio,
                               up_bankers[0].start_game_id, up_bankers[0].start_round_id, 1, sys_balance_asset);
    //asset cur_max_bet;                    //单人投注最大额
    // uint32_t banker_num;                    //上庄总人数
    //        asset system_investment;            //系统投资额
    bankbulls::global_state max_bet = models::global_state::find(contract, name(LIMIT_PLAY_BANK_KEY_NAME));
    uint64_t max_bet_amt = strtoull(max_bet.value.c_str(), NULL, 0);
    asset max_bet_asset = asset(max_bet_amt, EOS_SYMBOL);

    models::cur_round::update(contract, cur_round[0].id, max_bet_asset, cur_round[0].banker_num + 1, sys_balance_asset);
    return;
}

