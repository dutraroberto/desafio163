#include "bitcoin_utils.h"
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <sstream>
#include <iomanip>
#include <iostream>

// Initialize secp256k1 context
static secp256k1_context* secp256k1_ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

class ECCCache {
public:
    static secp256k1_context* getContext() {
        return secp256k1_ctx;
    }
};

std::vector<uint8_t> BitcoinUtils::sha256(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(hash.data(), &sha256);
    return hash;
}

std::vector<uint8_t> BitcoinUtils::ripemd160(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> hash(RIPEMD160_DIGEST_LENGTH);
    RIPEMD160_CTX ripemd160;
    RIPEMD160_Init(&ripemd160);
    RIPEMD160_Update(&ripemd160, data.data(), data.size());
    RIPEMD160_Final(hash.data(), &ripemd160);
    return hash;
}

std::string BitcoinUtils::privateKeyToWIF(const std::vector<uint8_t>& privateKey) {
    std::vector<uint8_t> extended;
    extended.push_back(0x80); // Mainnet private key prefix
    extended.insert(extended.end(), privateKey.begin(), privateKey.end());
    
    auto hash = sha256(extended);
    hash = sha256(hash);
    
    extended.insert(extended.end(), hash.begin(), hash.begin() + 4);
    return base58Encode(extended);
}

std::string BitcoinUtils::privateKeyToAddress(const std::vector<uint8_t>& privateKey) {
    secp256k1_pubkey pubkey;
    if (!secp256k1_ec_pubkey_create(ECCCache::getContext(), &pubkey, privateKey.data())) {
        throw std::runtime_error("Failed to create public key");
    }

    std::vector<uint8_t> pub_key_bytes(65);
    size_t pub_len = 65;
    if (!secp256k1_ec_pubkey_serialize(ECCCache::getContext(), pub_key_bytes.data(), &pub_len, &pubkey, SECP256K1_EC_UNCOMPRESSED)) {
        throw std::runtime_error("Failed to serialize public key");
    }

    // Hash SHA256
    std::vector<uint8_t> sha256_hash = sha256(pub_key_bytes);
    
    // Hash RIPEMD160
    std::vector<uint8_t> ripemd160_hash = ripemd160(sha256_hash);

    // Adicionar versão
    std::vector<uint8_t> versioned_hash;
    versioned_hash.reserve(21); // 1 byte version + 20 bytes hash
    versioned_hash.push_back(0x00); // Mainnet
    versioned_hash.insert(versioned_hash.end(), ripemd160_hash.begin(), ripemd160_hash.end());

    // Double SHA256 para checksum
    std::vector<uint8_t> checksum = sha256(sha256(versioned_hash));
    
    // Adicionar os primeiros 4 bytes do checksum
    versioned_hash.insert(versioned_hash.end(), checksum.begin(), checksum.begin() + 4);

    // Converter para Base58
    return base58Encode(versioned_hash);
}

std::vector<uint8_t> BitcoinUtils::hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    bytes.reserve(hex.length() / 2);
    
    try {
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
            bytes.push_back(byte);
        }
    } catch (const std::exception& e) {
        std::cout << "Erro ao converter hex para bytes: " << e.what() << std::endl;
        std::cout << "String hex: " << hex << std::endl;
        throw;
    }
    return bytes;
}

std::string BitcoinUtils::bytesToHexString(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t byte : data) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string BitcoinUtils::base58Encode(const std::vector<uint8_t>& data) {
    const char* ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    const int BASE = 58;

    std::vector<uint8_t> digits((data.size() * 138 / 100) + 1, 0);
    size_t digitsLen = 1;

    for (size_t i = 0; i < data.size(); i++) {
        uint32_t carry = data[i];
        for (size_t j = 0; j < digitsLen; j++) {
            carry += static_cast<uint32_t>(digits[j]) << 8;
            digits[j] = carry % BASE;
            carry /= BASE;
        }

        while (carry > 0) {
            digits[digitsLen++] = carry % BASE;
            carry /= BASE;
        }
    }

    std::string result;
    for (size_t i = 0; i < data.size() && data[i] == 0; i++) {
        result += ALPHABET[0];
    }

    for (size_t i = 0; i < digitsLen; i++) {
        result += ALPHABET[digits[digitsLen - 1 - i]];
    }

    return result;
}
