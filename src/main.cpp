#include "key_finder.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <limits>
#include <conio.h>

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string getValidInput(const std::string& prompt, bool allowEmpty = false) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        
        if (!allowEmpty && input.empty()) {
            std::cout << "Erro: Este campo nao pode estar vazio. Por favor, tente novamente.\n";
            continue;
        }
        return input;
    }
}

int getThreadCount() {
    int maxThreads = std::thread::hardware_concurrency();
    int threads;
    
    while (true) {
        std::cout << "Numero de nucleos disponiveis: " << maxThreads << "\n";
        std::cout << "Digite o numero de nucleos a utilizar (1-" << maxThreads << "): ";
        
        if (std::cin >> threads) {
            if (threads >= 1 && threads <= maxThreads) {
                clearInputBuffer();
                return threads;
            }
        }
        
        std::cout << "Erro: Por favor, digite um numero entre 1 e " << maxThreads << "\n";
        clearInputBuffer();
    }
}

void displayCommands() {
    std::cout << "\n------------------------------------------\n";
    std::cout << "Comandos disponiveis:\n";
    std::cout << "P - Pausar/Continuar\n";
    std::cout << "Q - Sair\n";
    std::cout << "------------------------------------------\n\n";
}

void displayProgress(KeyFinder& finder) {
    auto startTime = std::chrono::steady_clock::now();
    uint64_t lastCount = 0;
    uint64_t peakSpeed = 0;
    bool isPaused = false;

    displayCommands();

    while (finder.isRunning()) {
        // Verifica se há tecla pressionada
        if (_kbhit()) {
            char key = toupper(_getch());
            if (key == 'P') {
                isPaused = !isPaused;
                if (isPaused) {
                    std::cout << "\n[PAUSADO] Pressione 'P' para continuar ou 'Q' para sair\n";
                    finder.pause();
                } else {
                    std::cout << "\n[CONTINUANDO] Busca em andamento...\n";
                    finder.resume();
                }
            }
            else if (key == 'Q') {
                std::cout << "\nFinalizando busca...\n";
                finder.stop();
                break;
            }
        }

        if (!isPaused) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            
            uint64_t currentSpeed = finder.getTestsPerSecond();
            peakSpeed = std::max(peakSpeed, currentSpeed);

            std::cout << "\rVelocidade: " << currentSpeed << " testes/s"
                      << " | Pico: " << peakSpeed << " testes/s"
                      << " | Tempo: " << elapsedSeconds << "s";
            std::cout.flush();
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Desafio163 - Bitcoin Private Key Recovery Tool ===\n\n";

    // Obter endereco Bitcoin
    std::string address = getValidInput("Digite o endereco Bitcoin alvo: ");
    if (address.empty()) {
        std::cout << "Erro: O endereco Bitcoin e obrigatorio.\n";
        return 1;
    }

    // Obter chave parcial
    std::string partialKey = getValidInput("Digite a chave parcial (use 'x' para digitos desconhecidos): ");
    if (partialKey.empty()) {
        std::cout << "Erro: A chave parcial e obrigatoria.\n";
        return 1;
    }

    // Obter número de threads
    int threadCount = getThreadCount();

    std::cout << "\nIniciando busca com os seguintes parametros:\n";
    std::cout << "Endereco Bitcoin: " << address << "\n";
    std::cout << "Chave Parcial: " << partialKey << "\n";
    std::cout << "Nucleos: " << threadCount << "\n\n";

    try {
        KeyFinder finder(address, partialKey);
        finder.start(threadCount);
        displayProgress(finder);
    }
    catch (const std::exception& e) {
        std::cout << "\nErro: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
