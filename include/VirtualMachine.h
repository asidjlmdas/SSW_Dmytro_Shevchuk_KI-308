#pragma once
#include "ProcessManager.h"
#include "Logger.h"
#include <string>

namespace vmpm {

enum class VMState {
    STOPPED,
    RUNNING,
    PAUSED
};

class VirtualMachine {
public:
    explicit VirtualMachine(const std::string& name);
    ~VirtualMachine() = default;

    // Керування VM
    bool start();
    bool stop();
    bool pause();
    bool resume();

    // Стан
    VMState getState() const;
    const std::string& getName() const;
    bool isRunning() const;

    // Доступ до менеджера процесів
    ProcessManager& getProcessManager();
    const ProcessManager& getProcessManager() const;

    // Утиліти
    void printStatus() const;

private:
    std::string m_name;
    VMState m_state;
    ProcessManager m_processManager;
};

} // namespace vmpm
