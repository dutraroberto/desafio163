#define _CRT_SECURE_NO_WARNINGS
#include "key_finder.h"
#include "bitcoin_utils.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <omp.h> // Adicionado para suporte a OpenMP

const char HEX_CHARS[] = "0123456789ABCDEF";

// Definição da constante de classe
constexpr char KeyFinder::HEX_CHARS[];
std::mutex outputMutex;

std::string KeyFinder::getTimestamp(bool forFilename) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;

    if (forFilename) {
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    }
    else {
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    }

    return ss.str();
}

void KeyFinder::initLookupTables() {
    // Pré-computar todas as combinações possíveis de 2 caracteres hex
#pragma omp parallel for
    for (int i = 0; i < 256; ++i) {
        hexLookup_[i] = std::string(1, HEX_CHARS[(i >> 4) & 0xF]) + HEX_CHARS[i & 0xF];
    }
}

KeyFinder::KeyFinder(const std::string& targetAddress, const std::string& partialKey, size_t threadCount)
    : targetAddress_(targetAddress)
    , partialKey_(partialKey)
    , running_(false)
    , paused_(false)
    , testsCount_(0)
    , lastTestsCount_(0)
    , currentTotal_(0)
    , lastUpdate_(std::chrono::steady_clock::now())
    , startTime_(std::chrono::steady_clock::now())
    , startTimeStr_(getTimestamp(false))  // Armazena o horário de início
    , peakSpeed_(0)
    , checkpointFile_("checkpoint.dat") {

    // Inicializar posições dos X's
    for (size_t i = 0; i < partialKey_.length(); i++) {
        if (partialKey_[i] == 'x' || partialKey_[i] == 'X') {
            xPositions_.push_back(i);
        }
    }

    // Inicializar lookup tables
    initLookupTables();

    // Inicializar threads
    threads_.resize(threadCount);
}

KeyFinder::~KeyFinder() {
    stop();
}

void KeyFinder::start() {
    if (running_) return;

    running_ = true;
    paused_ = false;
    testsCount_.store(0, std::memory_order_relaxed);
    lastTestsCount_ = 0;
    currentTotal_ = 0;
    startTime_ = std::chrono::steady_clock::now();
    startTimeStr_ = getTimestamp(false);  // Armazena o horário de início
    lastUpdate_ = startTime_;
    peakSpeed_ = 0;

    // Iniciar threads
    for (size_t i = 0; i < threads_.size(); ++i) {
        threads_[i] = std::thread(&KeyFinder::workerThread, this, i);
    }
}

void KeyFinder::stop() {
    running_ = false;

    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
}

void KeyFinder::pause() {
    paused_ = true;
}

void KeyFinder::resume() {
    paused_ = false;
}

uint64_t KeyFinder::getTestsPerSecond() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate_).count();
    if (elapsed == 0) return 0;

    uint64_t current = testsCount_.load(std::memory_order_relaxed);
    return (current - lastTestsCount_) / elapsed;
}

void KeyFinder::saveCheckpoint(uint64_t currentIndex) {
    std::lock_guard<std::mutex> lock(checkpointMutex_);
    std::ofstream file(checkpointFile_, std::ios::binary);
    if (file) {
        file.write(reinterpret_cast<const char*>(&currentIndex), sizeof(currentIndex));
        file.write(reinterpret_cast<const char*>(&testsCount_), sizeof(testsCount_));
    }
}

uint64_t KeyFinder::loadCheckpoint() {
    std::lock_guard<std::mutex> lock(checkpointMutex_);
    std::ifstream file(checkpointFile_, std::ios::binary);
    uint64_t currentIndex = 0;
    if (file) {
        file.read(reinterpret_cast<char*>(&currentIndex), sizeof(currentIndex));
        uint64_t savedTestsCount;
        file.read(reinterpret_cast<char*>(&savedTestsCount), sizeof(savedTestsCount));
        testsCount_ = savedTestsCount;
    }
    return currentIndex;
}

void KeyFinder::updateStatistics() {
    std::lock_guard<std::mutex> lock(outputMutex);  // Bloqueia o acesso ao terminal
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate_).count();

    if (elapsed >= 1000) {  // Atualiza a cada segundo
        uint64_t currentTotal = testsCount_.load(std::memory_order_relaxed);
        double totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
        uint64_t averageSpeed = totalElapsed > 0 ? currentTotal / totalElapsed : 0;

        std::cout << "\033[2J\033[H";

        std::cout << "⚡ Search Progress ⚡\n";
        std::cout << "┌────────────────────────────────────────────────┐\n";
        std::cout << "│ Speed: " << std::setw(20) << averageSpeed << " keys/s             │\n";
        std::cout << "│ Keys:  " << std::setw(20) << currentTotal << "                    │\n";
        std::cout << "│ Time:  " << std::setw(20) << totalElapsed << " seconds            │\n";
        std::cout << "└────────────────────────────────────────────────┘\n";

        std::cout.flush();
        lastUpdate_ = now;
    }
}

void KeyFinder::foundKey(const std::string& privateKey, const std::string& address) {
    
    auto endTime = std::chrono::steady_clock::now();
    auto totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime_).count();
    uint64_t totalKeys = testsCount_.load(std::memory_order_relaxed);
    uint64_t averageSpeed = totalElapsed > 0 ? totalKeys / totalElapsed : 0;

    // Converter para WIF
    auto bytes = BitcoinUtils::hexStringToBytes(privateKey);
    std::string wif = BitcoinUtils::privateKeyToWIF(bytes);

    // Criar diretório de resultados na raiz do projeto
    std::filesystem::create_directories("../../resultados");

    // Criar nome do arquivo com timestamp
    std::string filename = "../../resultados/key_found_" + getTimestamp(true) + ".txt";

    // Salvar resultados no arquivo
    std::ofstream outFile(filename);
    outFile << "Bitcoin Key Found!\n";
    outFile << "================\n\n";
    outFile << "Private Key (HEX): " << privateKey << "\n";
    outFile << "Private Key (WIF): " << wif << "\n";
    outFile << "Address: " << address << "\n\n";
    outFile << "Search Statistics:\n";
    outFile << "----------------\n";
    outFile << "Start Time: " << startTimeStr_ << "\n";
    outFile << "End Time: " << getTimestamp(false) << "\n";
    outFile << "Total Keys Tested: " << totalKeys << "\n";
    outFile << "Average Speed: " << averageSpeed << " keys/s\n";
    outFile << "Total Time: " << totalElapsed << " seconds\n";
    outFile.close();

    // Limpar a tela de estatísticas
    std::cout << "\033[2J\033[H";

    // Exibir resultados na tela com título verde e caixa maior
    std::cout << "\n\n\033[32m🎉 Key Found! 🎉\033[0m\n\n";
    std::cout << "┌─────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│                     \033[32m🔑 Key Details 🔑\033[0m                                       │\n";
    std::cout << "├─────────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Private Key (HEX):                                                          │\n";
    std::cout << "│ " << std::left << std::setw(72) << privateKey << "    │\n";
    std::cout << "│                                                                             │\n";
    std::cout << "│ Private Key (WIF):                                                          │\n";
    std::cout << "│ " << std::left << std::setw(72) << wif << "    │\n";
    std::cout << "│                                                                             │\n";
    std::cout << "│ Address:                                                                    │\n";
    std::cout << "│ " << std::left << std::setw(72) << address << "    │\n";
    std::cout << "├─────────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│                          Search Statistics                                  │\n";
    std::cout << "├─────────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Start Time:        " << std::left << std::setw(55) << startTimeStr_ << "  │\n";
    std::cout << "│ End Time:          " << std::left << std::setw(55) << getTimestamp(false) << "  │\n";
    std::cout << "│ Total Keys Tested: " << std::left << std::setw(55) << totalKeys << "  │\n";
    std::cout << "│ Average Speed:     " << std::left << std::setw(53) << averageSpeed << " k/s│\n";
    std::cout << "│ Total Time:        " << std::left << std::setw(53) << totalElapsed << " s  │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────────────┘\n\n";
    std::cout << "Results saved to: " << filename << "\n";
    std::cout << "\nPress Enter to exit...\n";
    
    std::cin.get();
}

void KeyFinder::workerThread(size_t threadId) {
    const uint64_t totalCombinations = static_cast<uint64_t>(std::pow(16, xPositions_.size()));
    const uint64_t combinationsPerThread = totalCombinations / threads_.size();
    const uint64_t startIndex = threadId * combinationsPerThread;
    const uint64_t endIndex = (threadId == threads_.size() - 1) ? totalCombinations : startIndex + combinationsPerThread;

    std::vector<std::string> localBatchCandidates;
    localBatchCandidates.reserve(BATCH_SIZE);
    std::string foundKey;
    bool keyFound = false;
    uint64_t currentTotal = 0;
    auto lastStatsUpdate = std::chrono::steady_clock::now();

    for (uint64_t i = startIndex; running_ && i < endIndex; i++) {

        if (!running_) break;  // Verificação imediata

        if (paused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Gerar chave candidata
        std::string candidate = partialKey_;
        uint64_t combination = i;

        for (size_t pos : xPositions_) {
            uint8_t nibble = combination & 0xF;
            candidate[pos] = HEX_CHARS[nibble];
            combination >>= 4;
        }

        localBatchCandidates.push_back(candidate);
        testsCount_.fetch_add(1, std::memory_order_relaxed);

        if (localBatchCandidates.size() >= BATCH_SIZE) {
            // Validar batch de chaves
            for (const auto& key : localBatchCandidates) {
                if (validateKey(key)) {
                    foundKey = key;
                    running_ = false;  // Atualiza running_ global
                    std::string address = BitcoinUtils::privateKeyToAddress(BitcoinUtils::hexStringToBytes(foundKey));
                    this->foundKey(foundKey, address);
                    break;
                }
            }

            localBatchCandidates.clear();

            // Atualizar estatísticas
            currentTotal = testsCount_.load(std::memory_order_relaxed);
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastStatsUpdate).count();
            if (elapsed >= 1) {
                updateStatistics();
                lastStatsUpdate = now;
            }

        }
    }

    // Se encontrou a chave, processar o resultado
    if (keyFound) {
        
    }
    
}

bool KeyFinder::validateKey(const std::string& candidateKey) {
    try {
        // Converter a chave para bytes
        auto bytes = BitcoinUtils::hexStringToBytes(candidateKey);

        // Gerar endereço Bitcoin
        std::string address = BitcoinUtils::privateKeyToAddress(bytes);

        // Verificar se o endereço corresponde ao alvo
        if (address == targetAddress_) {
            return true;
        }

        return false;
    }
    catch (const std::exception& e) {
        // Em caso de erro na conversão ou geração do endereço
        std::cerr << "Erro ao validar chave: " << e.what() << std::endl;
        return false;
    }
}