#include "key_finder.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <limits>
#include <conio.h>
#include <Windows.h>

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
            std::cout << "Error: This field cannot be empty. Please try again.\n";
            continue;
        }
        return input;
    }
}

int getThreadCount() {
    int maxThreads = std::thread::hardware_concurrency();
    int threads;
    
    while (true) {
        std::cout << "Number of available cores: " << maxThreads << "\n";
        std::cout << "Enter the number of cores to use (1-" << maxThreads << "): ";
        
        if (std::cin >> threads) {
            if (threads >= 1 && threads <= maxThreads) {
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
    displayCommands();
    
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
    SetConsoleOutputCP(CP_UTF8);
    
    std::cout << "Bitcoin Private Key Finder\n";
    std::cout << "=========================\n\n";
    
    std::string address, partialKey;
    int threadCount;
    
    std::cout << "Target Bitcoin Address: ";
    std::getline(std::cin, address);
    
    std::cout << "Partial Private Key (use 'x' for unknown positions): ";
    std::getline(std::cin, partialKey);
    
    std::cout << "Number of threads to use: ";
    std::cin >> threadCount;
    
    if (threadCount <= 0) {
        threadCount = std::thread::hardware_concurrency();
    }
    
    std::cout << "\nStarting search with parameters:\n";
    std::cout << "Address: " << address << "\n";
    std::cout << "Key Pattern: " << partialKey << "\n";
    std::cout << "Threads: " << threadCount << "\n\n";
    
    try {
        KeyFinder finder(address, partialKey, threadCount);
        finder.start();
        displayProgress(finder);
    }
    catch (const std::exception& e) {
        std::cout << "\nError: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
