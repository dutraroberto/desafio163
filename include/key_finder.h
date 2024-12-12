#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <array>
#ifdef _OPENMP
#include <omp.h>
#endif

class KeyFinder {
public:
    KeyFinder(const std::string& targetAddress, const std::string& partialKey, size_t threadCount);
    ~KeyFinder();

    void start();
    void stop();
    void pause();
    void resume();
    bool isPaused() const { return paused_; }
    bool isRunning() const { return running_; }
    uint64_t getTestsPerSecond() const;
    
private:
    void updateStatistics();
    void foundKey(const std::string& privateKey, const std::string& address);
    void workerThread(size_t threadId);
    bool validateKey(const std::string& key);
    void saveCheckpoint(uint64_t position);
    uint64_t loadCheckpoint();
    std::string getTimestamp(bool forFilename = false);
    uint64_t generateCombination(size_t threadId, uint64_t index);
    void initLookupTables();
    
    std::string targetAddress_;
    std::string partialKey_;
    std::vector<size_t> xPositions_;
    std::atomic<bool> running_;
    std::atomic<bool> paused_;
    std::atomic<uint64_t> testsCount_;
    std::atomic<uint64_t> peakSpeed_;
    std::vector<std::thread> threads_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point lastUpdate_;
    std::string startTimeStr_;  // Armazena o horário de início formatado
    uint64_t lastTestsCount_;
    uint64_t currentTotal_;
    std::string checkpointFile_;
    std::array<std::string, 256> hexLookup_;
    std::mutex checkpointMutex_;

    static constexpr size_t BATCH_SIZE = 1000;
    static constexpr uint64_t REPORT_INTERVAL = 10000;
    static constexpr uint64_t CHECKPOINT_INTERVAL = 1000000;
    static constexpr char HEX_CHARS[] = "0123456789ABCDEF";
};
