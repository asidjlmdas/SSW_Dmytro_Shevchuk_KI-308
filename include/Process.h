#pragma once
#include <string>
#include <chrono>

namespace vmpm {

enum class ProcessState {
    RUNNING,
    SUSPENDED,
    STOPPED,
    ZOMBIE
};

enum class ProcessPriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    REALTIME = 3
};

struct ProcessStats {
    double cpuUsage;      // відсоток CPU (0.0 - 100.0)
    size_t memoryBytes;   // використана пам'ять в байтах
    uint64_t uptimeMs;    // час роботи в мілісекундах
};

class Process {
public:
    Process(int pid, const std::string& name, int parentPid = -1);
    ~Process() = default;

    // Геттери
    int getPid() const;
    int getParentPid() const;
    const std::string& getName() const;
    ProcessState getState() const;
    ProcessPriority getPriority() const;
    ProcessStats getStats() const;
    std::string getStateString() const;

    // Керування станом
    bool suspend();
    bool resume();
    bool stop();
    bool kill();

    // Налаштування
    void setPriority(ProcessPriority priority);
    void updateStats(double cpu, size_t memory);

    // Перевірки
    bool isAlive() const;
    bool isZombie() const;
    uint64_t getUptimeMs() const;

private:
    int m_pid;
    int m_parentPid;
    std::string m_name;
    ProcessState m_state;
    ProcessPriority m_priority;
    ProcessStats m_stats;
    std::chrono::steady_clock::time_point m_startTime;
};

} // namespace vmpm
