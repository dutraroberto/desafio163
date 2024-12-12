#include "key_finder.h"
#include "bitcoin_utils.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>

KeyFinder::KeyFinder(const std::string& targetAddress, const std::string& partialKey)
    : targetAddress_(targetAddress)
    , partialKey_(partialKey)
    , running_(false)
    , testsCount_(0)
    , lastTestsCount_(0) {
}

void KeyFinder::start(size_t threadCount) {
    if (running_) return;
    
    running_ = true;
    testsCount_ = 0;
    lastTestsCount_ = 0;

    // Iniciar threads
    for (size_t i = 0; i < threadCount; ++i) {
        threads_.emplace_back(&KeyFinder::workerThread, this, i);
    }
}

void KeyFinder::stop() {
    running_ = false;
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads_.clear();
}

bool KeyFinder::isRunning() const {
    return running_;
}

uint64_t KeyFinder::getTestsPerSecond() const {
    return testsCount_ - lastTestsCount_;
}

void KeyFinder::workerThread(size_t threadId) {
    uint64_t startIndex = threadId;
    uint64_t step = threads_.size();

    while (running_) {
        std::string candidateKey = generateCandidateKey(startIndex);
        if (validateKey(candidateKey)) {
            std::cout << "\nChave encontrada: " << candidateKey << std::endl;
            
            // Converter para WIF e salvar
            auto bytes = BitcoinUtils::hexStringToBytes(candidateKey);
            std::string wif = BitcoinUtils::privateKeyToWIF(bytes);
            
            std::ofstream outFile("chave_encontrada.txt");
            outFile << "Chave privada (hex): " << candidateKey << std::endl;
            outFile << "Chave privada (WIF): " << wif << std::endl;
            outFile.close();

            stop();
            break;
        }

        testsCount_++;
        startIndex += step;
    }
}

bool KeyFinder::validateKey(const std::string& candidateKey) {
    try {
        auto bytes = BitcoinUtils::hexStringToBytes(candidateKey);
        std::string address = BitcoinUtils::privateKeyToAddress(bytes);
        
        if (address == targetAddress_) {
            std::cout << "\nChave válida encontrada!" << std::endl;
            std::cout << "Chave privada: " << candidateKey << std::endl;
            std::cout << "Endereço: " << address << std::endl;
            return true;
        }
    } catch (const std::exception& e) {
        // Ignora chaves inválidas silenciosamente
    }
    return false;
}

std::string KeyFinder::generateCandidateKey(uint64_t index) {
    std::string key = partialKey_;
    size_t xCount = 0;
    
    // Conta o número de 'x' na chave parcial
    for (char c : key) {
        if (c == 'x') xCount++;
    }
    
    // Gera uma sequência de dígitos hexadecimais baseada no índice
    std::vector<char> hexDigits;
    uint64_t remainingIndex = index;
    
    for (size_t i = 0; i < xCount; i++) {
        int digit = remainingIndex & 0xF;
        remainingIndex >>= 4;
        
        char hexChar;
        if (digit < 10) {
            hexChar = '0' + digit;
        } else {
            hexChar = 'a' + (digit - 10);
        }
        hexDigits.push_back(hexChar);
    }
    
    // Substitui os 'x' pelos dígitos gerados
    size_t digitIndex = 0;
    for (size_t i = 0; i < key.length() && digitIndex < hexDigits.size(); i++) {
        if (key[i] == 'x') {
            key[i] = hexDigits[digitIndex++];
        }
    }
    
    return key;
}
