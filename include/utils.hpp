#pragma once

#include <eosio/crypto.hpp>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/time.hpp>
#include <eosio/print.hpp>
#include <vector>
#include <string>


using namespace eosio;
using namespace std;


string uint_string(uint64_t input) {
    string result;
    uint8_t base = 10;
    do {
        char c = input % base;
        input /= base;
        if (c < 10)
            c += '0';
        else
            c += 'A' - 10;
        result = c + result;
    } while (input);
    return result;
}

uint8_t from_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    check(false, "Invalid hex character");
    return 0;
}

size_t from_hex(const string &hex_str, char *out_data, size_t out_data_len) {
    auto i = hex_str.begin();
    uint8_t *out_pos = (uint8_t *) out_data;
    uint8_t *out_end = out_pos + out_data_len;
    while (i != hex_str.end() && out_end != out_pos) {
        *out_pos = from_hex((char) (*i)) << 4;
        ++i;
        if (i != hex_str.end()) {
            *out_pos |= from_hex((char) (*i));
            ++i;
        }
        ++out_pos;
    }
    return out_pos - (uint8_t *) out_data;
}

string to_hex(const char *d, uint32_t s) {
    std::string r;
    const char *to_hex = "0123456789abcdef";
    uint8_t *c = (uint8_t *) d;
    for (uint32_t i = 0; i < s; ++i)
        (r += to_hex[(c[i] >> 4)]) += to_hex[(c[i] & 0x0f)];
    return r;
}

string sha256_to_hex(const checksum256 &sha256) {
    return to_hex((char *) sha256.extract_as_byte_array().data(), sizeof(sha256.extract_as_byte_array()));
}


// copied from boost https://www.boost.org/
template<class T>
inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

uint64_t uint64_hash(const string &hash) {
    return std::hash<string>{}(hash);
}

uint64_t uint64_hash(const checksum256 &hash) {
    return uint64_hash(sha256_to_hex(hash));
}

checksum256 hex_to_sha256(const string &hex_str) {
    check(hex_str.length() == 64, "invalid sha256");
    checksum256 checksum;
    from_hex(hex_str, (char *) checksum.extract_as_byte_array().data(), sizeof(checksum.extract_as_byte_array()));
    //from_hex(hex_str, (char*)checksum.hash, sizeof(checksum.hash));
    return checksum;
}

checksum160 hex_to_sha1(const string &hex_str) {
    check(hex_str.length() == 40, "invalid sha1");
    checksum160 checksum;
    from_hex(hex_str, (char *) checksum.extract_as_byte_array().data(), sizeof(checksum.extract_as_byte_array()));
    return checksum;
}

size_t sub2sep(const string &input,
               string *output,
               const char &separator,
               const size_t &first_pos = 0,
               const bool &required = false) {
    check(first_pos != string::npos, "invalid first pos");
    auto pos = input.find(separator, first_pos);
    if (pos == string::npos) {
        check(!required, "parse memo error");
        return string::npos;
    }
    *output = input.substr(first_pos, pos - first_pos);
    return pos;
}

// Copied from https://github.com/bitcoin/bitcoin

/** All alphanumeric characters except for "0", "I", "O", and "l" */
static const char *pszBase58 =
        "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const int8_t mapBase58[256] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7,
        8, -1, -1, -1, -1, -1, -1, -1, 9, 10, 11, 12, 13, 14, 15, 16, -1, 17, 18,
        19, 20, 21, -1, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1,
        -1, -1, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, -1, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

bool DecodeBase58(const char *psz, std::vector<unsigned char> &vch) {
    // Skip leading spaces.
    while (*psz && isspace(*psz)) psz++;
    // Skip and count leading '1's.
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') {
        zeroes++;
        psz++;
    }
    // Allocate enough space in big-endian base256 representation.
    int size = strlen(psz) * 733 / 1000 + 1;  // log(58) / log(256), rounded up.
    std::vector<unsigned char> b256(size);
    // Process the characters.
    static_assert(
            sizeof(mapBase58) / sizeof(mapBase58[0]) == 256,
            "mapBase58.size() should be 256");  // guarantee not out of range
    while (*psz && !isspace(*psz)) {
        // Decode base58 character
        int carry = mapBase58[(uint8_t) *psz];
        if (carry == -1)  // Invalid b58 character
            return false;
        int i = 0;
        for (std::vector<unsigned char>::reverse_iterator it = b256.rbegin();
             (carry != 0 || i < length) && (it != b256.rend());
             ++it, ++i) {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        psz++;
    }
    // Skip trailing spaces.
    while (isspace(*psz)) psz++;
    if (*psz != 0) return false;
    // Skip leading zeroes in b256.
    std::vector<unsigned char>::iterator it = b256.begin() + (size - length);
    while (it != b256.end() && *it == 0) it++;
    // Copy result into output vector.
    vch.reserve(zeroes + (b256.end() - it));
    vch.assign(zeroes, 0x00);
    while (it != b256.end()) vch.push_back(*(it++));
    return true;
}

bool decode_base58(const string &str, vector<unsigned char> &vch) {
    return DecodeBase58(str.c_str(), vch);
}

signature str_to_sig(string sig) {
    string prefix("SIG_K1_");
    sig = sig.substr(prefix.length());
    vector<unsigned char> vch;
    decode_base58(sig, vch);
    signature _sig;
    _sig.type = 0;
    //unsigned int type = 0;
    //_sig.data[0] = (uint8_t) type;
    for (int i = 0; i < sizeof(_sig.data); i++) {
        _sig.data[i] = vch[i];
    }
    return _sig;
}

public_key str_to_pub(string pubkey) {
    string pubkey_prefix("EOS");
    auto base58substr = pubkey.substr(pubkey_prefix.length());
    vector<unsigned char> vch;
    decode_base58(base58substr, vch);
    public_key _pub_key;
    _pub_key.type = 0;
    //_pub_key.data[0] = (char) type;
    for (int i = 0; i < sizeof(_pub_key.data); i++) {
        _pub_key.data[i] = vch[i];
    }
    return _pub_key;
}


public_key string_to_public_key(unsigned int const key_type, std::string const &public_key_str) {
    eosio::public_key public_key;
    public_key.type = key_type; // Could be K1 or R1 enum
    for (int i = 0; i < 33; ++i) {
        public_key.data.at(i) = public_key_str.at(i);
    }
    return public_key;
}

vector<string> split_string(const string &s, const string &c) {
    vector<string> v;
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

asset string_to_asset(string asset_str) {
    //symbol(symbol_code("EOS"),4)
    vector<string> asset_info = split_string(asset_str, " ");
    check(asset_info[1] == "EOS", "token must be EOS");
    vector<string> amount_info = split_string(asset_info[0], ".");
    int64_t amount = stoll(amount_info[0]) * 10000 + stoll(amount_info[1]); //对冲掉assst除以1000的影响
    return {amount, EOS_SYMBOL};
}

template<typename T>
void
delay_call_action(name contract, name fee_account, name action, T tuple_info, uint32_t delay_time, uint64_t sender_id) {
    eosio::transaction txn{};
    txn.actions.emplace_back(eosio::permission_level(fee_account, "active"_n), contract, action, tuple_info);
    txn.delay_sec = delay_time;
    txn.send(sender_id, fee_account, false);
    return;
}