#define _CRT_SECURE_NO_WARNINGS
#include "key_finder.h"
#include "bitcoin_utils.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <filesystem>
#include <ctime>

// Função auxiliar para formatar data/hora
std::string getTimestamp(bool isFileName = false) {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    
    char buffer[80];
    if (isFileName) {
        strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &timeinfo);
    } else {
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    }
    return std::string(buffer);
}

KeyFinder::KeyFinder(const std::string& targetAddress, const std::string& partialKey)
    : targetAddress_(targetAddress)
    , partialKey_(partialKey)
    , running_(false)
    , paused_(false)
    , testsCount_(0)
    , lastTestsCount_(0) {
}

void KeyFinder::start(size_t threadCount) {
    if (running_) return;
    
    running_ = true;
    paused_ = false;
    testsCount_ = 0;
    lastTestsCount_ = 0;

    std::cout << "Iniciando busca com " << threadCount << " threads...\n";
    std::cout << "Procurando chave para o endereco: " << targetAddress_ << "\n";
    std::cout << "Chave parcial: " << partialKey_ << "\n\n";

    // Iniciar threads
    for (size_t i = 0; i < threadCount; ++i) {
        threads_.emplace_back(&KeyFinder::workerThread, this, i);
    }
}

void KeyFinder::stop() {
    running_ = false;
    paused_ = false;
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads_.clear();
}

void KeyFinder::pause() {
    paused_ = true;
}

void KeyFinder::resume() {
    paused_ = false;
}

bool KeyFinder::isRunning() const {
    return running_;
}

uint64_t KeyFinder::getTestsPerSecond() const {
    return testsCount_ - lastTestsCount_;
}

void KeyFinder::workerThread(size_t threadId) {
    // Encontrar posições dos X's
    std::vector<size_t> xPositions;
    for (size_t i = 0; i < partialKey_.length(); i++) {
        if (partialKey_[i] == 'x' || partialKey_[i] == 'X') {
            xPositions.push_back(i);
        }
    }
    
    // Calcular número total de combinações
    uint64_t totalCombinations = static_cast<uint64_t>(std::pow(16, xPositions.size()));
    
    // Distribuir o trabalho entre as threads
    uint64_t combinationsPerThread = totalCombinations / threads_.size();
    uint64_t start = threadId * combinationsPerThread;
    uint64_t end = (threadId == threads_.size() - 1) ? totalCombinations : (threadId + 1) * combinationsPerThread;

    const char* hexChars = "0123456789abcdef";
    
    std::cout << "Thread " << threadId << " iniciando...\n";
    std::cout << "  Combinacoes: " << start << " ate " << end << "\n";
    std::cout << "  Total: " << (end - start) << " combinacoes\n\n";
    
    uint64_t lastReport = 0;
    const uint64_t reportInterval = 1000000; // Reportar a cada 1 milhão de tentativas
    
    for (uint64_t i = start; i < end && running_; i++) {
        while (paused_ && running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        if (!running_) break;

        // Gerar candidato
        std::string candidateKey = partialKey_;
        uint64_t value = i;
        
        // Preencher cada posição X com um dígito hexadecimal
        for (size_t pos = 0; pos < xPositions.size(); pos++) {
            int digit = value & 0xF;
            candidateKey[xPositions[pos]] = hexChars[digit];
            value >>= 4;
        }
        
        // Mostrar progresso a cada reportInterval tentativas
        if (i - lastReport >= reportInterval) {
            double progress = static_cast<double>(i - start) / (end - start) * 100.0;
            std::cout << "Thread " << threadId << ": " 
                     << (i - start) << "/" << (end - start) 
                     << " (" << std::fixed << std::setprecision(2) << progress << "%)"
                     << " - Testando: " << candidateKey << "\r" << std::flush;
            lastReport = i;
        }
        
        if (validateKey(candidateKey)) {
            std::cout << "\n==========================================\n";
            std::cout << "CHAVE ENCONTRADA!\n";
            std::cout << "==========================================\n";
            
            // Converter para WIF
            auto bytes = BitcoinUtils::hexStringToBytes(candidateKey);
            std::string wif = BitcoinUtils::privateKeyToWIF(bytes);
            std::string address = BitcoinUtils::privateKeyToAddress(bytes);
            
            // Exibir resultados
            std::cout << "Chave privada (HEX): " << candidateKey << "\n";
            std::cout << "Chave privada (WIF): " << wif << "\n";
            std::cout << "Endereco Bitcoin: " << address << "\n";
            std::cout << "==========================================\n";
            
            // Criar diretório results se não existir
            std::filesystem::create_directories("results");
            
            // Gerar nome do arquivo com timestamp
            std::string filename = "results/chave_encontrada_" + getTimestamp(true) + ".txt";
            
            // Salvar em arquivo
            std::ofstream outFile(filename);
            outFile << "=== Chave Bitcoin Encontrada ===\n\n";
            outFile << "Data/Hora: " << getTimestamp() << "\n\n";
            outFile << "Chave privada (HEX): " << candidateKey << "\n";
            outFile << "Chave privada (WIF): " << wif << "\n";
            outFile << "Endereco Bitcoin: " << address << "\n\n";
            outFile << "Detalhes da busca:\n";
            outFile << "- Chave parcial usada: " << partialKey_ << "\n";
            outFile << "- Endereco alvo: " << targetAddress_ << "\n";
            outFile << "- Numero de threads: " << threads_.size() << "\n\n";
            outFile << "==========================================\n\n";
            outFile << "Parabens pela sua conquista!\n";
            outFile << "Considere fazer uma pequena doacao para o desenvolvedor da ferramenta:\n\n";
            outFile << "Endereco BTC: bc1q3g8s6wh7s8zf4jz32msu2hl3wu9y4rt9nfrzgu\n\n";
            outFile << "Sua doacao ajuda a manter o desenvolvimento de ferramentas uteis como esta.\n";
            outFile << "Muito obrigado pelo seu apoio!\n\n";
            outFile << "==========================================\n";
            outFile.close();
            
            std::cout << "\nChave salva em: " << filename << "\n";
            std::cout << "Pressione Enter para continuar...";
            std::cin.get();

            stop();
            break;
        }

        testsCount_++;
    }
    
    // Limpar a linha do progresso ao finalizar
    std::cout << std::string(100, ' ') << "\r" << std::flush;
    std::cout << "Thread " << threadId << " finalizada.\n";
}

bool KeyFinder::validateKey(const std::string& candidateKey) {
    try {
        // Converter a chave para bytes
        auto privateKeyBytes = BitcoinUtils::hexStringToBytes(candidateKey);
        
        // Verificar se o tamanho está correto (32 bytes)
        if (privateKeyBytes.size() != 32) {
            return false;
        }
        
        // Gerar o endereço Bitcoin
        std::string generatedAddress = BitcoinUtils::privateKeyToAddress(privateKeyBytes);
        
        // Comparar com o endereço alvo
        bool found = (generatedAddress == targetAddress_);
        
        if (found) {
            std::cout << "\n!!! CHAVE ENCONTRADA !!!\n";
            std::cout << "Chave privada: " << candidateKey << "\n";
            std::cout << "Endereco Bitcoin: " << generatedAddress << "\n\n";
        }
        
        return found;
    } catch (const std::exception& e) {
        return false;
    }
}
