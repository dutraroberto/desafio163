#pragma once
#include <string>
#include <vector>

class BitcoinUtils {
public:
    static std::string privateKeyToWIF(const std::vector<uint8_t>& privateKey);
    static std::string privateKeyToAddress(const std::vector<uint8_t>& privateKey);
    static std::vector<uint8_t> hexStringToBytes(const std::string& hex);
    static std::string bytesToHexString(const std::vector<uint8_t>& data);
    
private:
    static std::vector<uint8_t> sha256(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> ripemd160(const std::vector<uint8_t>& data);
    static std::string base58Encode(const std::vector<uint8_t>& data);
};
