#include "key_finder.h"
#include <iostream>
#include <string>
#include <thread>
#include <conio.h>
#include <Windows.h>

// Definir NOMINMAX antes de qualquer include do Windows
#define NOMINMAX
#undef max
#undef min

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

std::string getValidInput(const std::string& prompt, bool allowEmpty = false) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        
        if (!allowEmpty && input.empty()) {
            std::cout << "Error: This field cannot be empty. Please try again.\n";
            continue;
        }
        return input;
    }
}

int getThreadCount() {
    unsigned int maxThreads = std::thread::hardware_concurrency();
    int threads;
    
    while (true) {
        std::cout << "Number of available cores: " << maxThreads << "\n";
        std::cout << "Enter the number of cores to use (1-" << maxThreads << "): ";
        
        if (std::cin >> threads) {
            if (threads >= 1 && threads <= static_cast<int>(maxThreads)) {
                clearInputBuffer();
                return threads;
            }
        }
        
        std::cout << "Error: Please enter a number between 1 and " << maxThreads << "\n";
        clearInputBuffer();
    }
}

void displayCommands() {
    std::cout << "Commands available:\n";
    std::cout << "P - Pause/Resume search\n";
    std::cout << "Q - Quit program\n\n";
}

void displayProgress(KeyFinder& finder) {
    while (finder.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (_kbhit()) {
            char c = _getch();
            if (c == 'q' || c == 'Q') {
                std::cout << "\nStopping search..." << std::endl;
                finder.stop();
                break;
            }
            else if (c == 'p' || c == 'P') {
                if (finder.isPaused()) {
                    finder.resume();
                    std::cout << "\nSearch resumed." << std::endl;
                }
                else {
                    finder.pause();
                    std::cout << "\nSearch paused." << std::endl;
                }
            }
        }
    }
    std::cout << "\nSearch finished." << std::endl;
}

int main() {
    // Configurar codificação UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // Configurar console para suportar sequências ANSI
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    std::cout << "\n";
    std::cout << "Bitcoin Private Key Finder\n";
    std::cout << "=========================\n";
    std::cout << "\n";
    
    // Entrada do endereço Bitcoin
    std::cout << "Target Bitcoin Address:\n";
    std::cout << "> ";
    std::string address;
    std::getline(std::cin, address);
    std::cout << "\n";
    
    // Entrada da chave parcial
    std::cout << "Partial Private Key:\n";
    std::cout << "(use 'x' for unknown positions)\n";
    std::cout << "> ";
    std::string partialKey;
    std::getline(std::cin, partialKey);
    std::cout << "\n";
    
    // Seleção do número de threads
    int threadCount = getThreadCount();
    std::cout << "\n";
    
    // Exibição dos comandos disponíveis
    displayCommands();
    std::cout << "\n";
    
    std::cout << "Starting search...\n";
    std::cout << "==================\n\n";
    
    try {
        KeyFinder finder(address, partialKey, threadCount);
        finder.start();
        displayProgress(finder);
    }
    catch (const std::exception& e) {
        std::cout << "\nError:\n";
        std::cout << e.what() << "\n\n";
        return 1;
    }
    
    return 0;
}
