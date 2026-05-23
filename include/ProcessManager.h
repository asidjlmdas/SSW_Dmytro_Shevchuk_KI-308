#pragma once
#include "Process.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace vmpm {

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager() = default;

    // Запуск / зупинка процесів
    int  startProcess(const std::string& name, int parentPid = -1);
    bool stopProcess(int pid);
    bool killProcess(int pid);
    bool suspendProcess(int pid);
    bool resumeProcess(int pid);

    // Отримання інформації
    std::shared_ptr<Process> findByPid(int pid) const;
    std::vector<std::shared_ptr<Process>> findByName(const std::string& name) const;
    std::vector<std::shared_ptr<Process>> getAllProcesses() const;
    std::vector<std::shared_ptr<Process>> getChildren(int parentPid) const;

    // Пріоритет
    bool setPriority(int pid, ProcessPriority priority);

    // Моніторинг
    void updateStats();
    void killZombies();
    void killByTimeout(uint64_t maxUptimeMs);

    // Виведення
    void printProcessList() const;
    void printProcessTree(int rootPid = -1, int indent = 0) const;

    // Логування
    bool saveLogsToFile(const std::string& filename) const;

private:
    std::vector<std::shared_ptr<Process>> m_processes;
    int m_nextPid;

    std::shared_ptr<Process> findProcess(int pid) const;
    void addLogEntry(const std::string& message);
    std::vector<std::string> m_log;
};

} // namespace vmpm
