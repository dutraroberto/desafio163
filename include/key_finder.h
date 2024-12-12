#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

class KeyFinder {
public:
    KeyFinder(const std::string& targetAddress, const std::string& partialKey);
    
    void start(size_t threadCount);
    void stop();
    void pause();
    void resume();
    bool isRunning() const;
    uint64_t getTestsPerSecond() const;

private:
    void workerThread(size_t threadId);
    bool validateKey(const std::string& candidateKey);
    std::string generateCandidateKey(uint64_t index);

    std::string targetAddress_;
    std::string partialKey_;
    std::vector<std::thread> threads_;
    std::atomic<bool> running_;
    std::atomic<bool> paused_;
    std::atomic<uint64_t> testsCount_;
    std::atomic<uint64_t> lastTestsCount_;
    std::chrono::steady_clock::time_point lastUpdate_;
};
