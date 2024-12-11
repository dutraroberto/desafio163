#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "key_finder.h"

struct Config {
    std::string targetAddress;
    std::string partialKey;
    size_t threadCount;
    bool benchmark;
    
    static Config parseCommandLine(int argc, char* argv[]) {
        Config config;
        config.targetAddress = "1Hoyt6UBzwL5vvUSTLMQC2mwvvE5PpeSC";
        config.partialKey = "4x3x3x4xcxfx6x9xfx3xaxcx5x0x4xbxbx7x2x6x8x7x8xax4x0x8x3x3x3x7x3x";
        config.threadCount = std::thread::hardware_concurrency();
        config.benchmark = false;
        
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--address") == 0 && i + 1 < argc) {
                config.targetAddress = argv[++i];
            }
            else if (strcmp(argv[i], "--key") == 0 && i + 1 < argc) {
                config.partialKey = argv[++i];
            }
            else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
                config.threadCount = std::stoul(argv[++i]);
            }
            else if (strcmp(argv[i], "--benchmark") == 0) {
                config.benchmark = true;
            }
            else if (strcmp(argv[i], "--help") == 0) {
                std::cout << "Uso: Desafio163.exe [opções]\n"
                         << "Opções:\n"
                         << "  --address <endereço>  Endereço Bitcoin alvo\n"
                         << "  --key <chave>         Chave privada parcial\n"
                         << "  --threads <número>    Número de threads (padrão: auto)\n"
                         << "  --benchmark           Executar teste de performance\n"
                         << "  --help                Mostrar esta ajuda\n";
                exit(0);
            }
        }
        
        if (config.threadCount == 0) config.threadCount = 4;
        return config;
    }
};

void printProgress(uint64_t testsPerSecond, const std::chrono::steady_clock::time_point& startTime) {
    static auto lastUpdate = std::chrono::steady_clock::now();
    static uint64_t totalTests = 0;
    static uint64_t peakSpeed = 0;
    
    totalTests += testsPerSecond;
    peakSpeed = std::max(peakSpeed, testsPerSecond);
    
    auto now = std::chrono::steady_clock::now();
    auto totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count();
    
    std::cout << "\rVelocidade: " << std::setw(10) << testsPerSecond << " chaves/s"
              << " | Pico: " << std::setw(10) << peakSpeed << " chaves/s"
              << " | Total: " << std::setw(15) << totalTests
              << " | Tempo: " << std::setw(5) << totalElapsed << "s" << std::flush;
              
    if (elapsed >= 10) {
        lastUpdate = now;
        std::cout << std::endl;
    }
}

void runBenchmark() {
    std::cout << "\nExecutando benchmark...\n";
    const int duration = 5; // segundos
    
    KeyFinder finder("1Hoyt6UBzwL5vvUSTLMQC2mwvvE5PpeSC", 
                    "4x3x3x4xcxfx6x9xfx3xaxcx5x0x4xbxbx7x2x6x8x7x8xax4x0x8x3x3x3x7x3x");
    
    auto startTime = std::chrono::steady_clock::now();
    finder.start(std::thread::hardware_concurrency());
    
    uint64_t maxSpeed = 0;
    while (std::chrono::duration_cast<std::chrono::seconds>(
           std::chrono::steady_clock::now() - startTime).count() < duration) {
        uint64_t speed = finder.getTestsPerSecond();
        maxSpeed = std::max(maxSpeed, speed);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    finder.stop();
    
    std::cout << "\nResultados do benchmark:\n"
              << "Velocidade máxima: " << maxSpeed << " chaves/s\n"
              << "Threads utilizadas: " << std::thread::hardware_concurrency() << "\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Desafio163 - Bitcoin Private Key Finder v2.0\n"
              << "----------------------------------------\n";

    Config config = Config::parseCommandLine(argc, argv);
    
    if (config.benchmark) {
        runBenchmark();
        return 0;
    }

    std::cout << "Configuração:\n"
              << "- Threads: " << config.threadCount << "\n"
              << "- Endereço alvo: " << config.targetAddress << "\n"
              << "- Chave parcial: " << config.partialKey << "\n"
              << "\nIniciando busca...\n";

    KeyFinder finder(config.targetAddress, config.partialKey);
    finder.start(config.threadCount);

    auto startTime = std::chrono::steady_clock::now();

    while (finder.isRunning()) {
        uint64_t currentSpeed = finder.getTestsPerSecond();
        printProgress(currentSpeed, startTime);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

    std::cout << "\n\nBusca finalizada!\n"
              << "Tempo total: " << duration << " segundos\n";

    return 0;
}
