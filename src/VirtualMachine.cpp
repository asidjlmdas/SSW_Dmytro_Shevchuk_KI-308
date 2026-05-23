#include "VirtualMachine.h"
#include <iostream>

namespace vmpm {

VirtualMachine::VirtualMachine(const std::string& name)
    : m_name(name), m_state(VMState::STOPPED)
{}

bool VirtualMachine::start() {
    if (m_state == VMState::RUNNING) return false;
    m_state = VMState::RUNNING;
    Logger::instance().info("VM '" + m_name + "' started");
    return true;
}

bool VirtualMachine::stop() {
    if (m_state == VMState::STOPPED) return false;
    m_state = VMState::STOPPED;
    Logger::instance().info("VM '" + m_name + "' stopped");
    return true;
}

bool VirtualMachine::pause() {
    if (m_state != VMState::RUNNING) return false;
    m_state = VMState::PAUSED;
    Logger::instance().info("VM '" + m_name + "' paused");
    return true;
}

bool VirtualMachine::resume() {
    if (m_state != VMState::PAUSED) return false;
    m_state = VMState::RUNNING;
    Logger::instance().info("VM '" + m_name + "' resumed");
    return true;
}

VMState VirtualMachine::getState() const { return m_state; }
const std::string& VirtualMachine::getName() const { return m_name; }
bool VirtualMachine::isRunning() const { return m_state == VMState::RUNNING; }

ProcessManager& VirtualMachine::getProcessManager() { return m_processManager; }
const ProcessManager& VirtualMachine::getProcessManager() const { return m_processManager; }

void VirtualMachine::printStatus() const {
    std::string stateStr;
    switch (m_state) {
        case VMState::STOPPED: stateStr = "STOPPED"; break;
        case VMState::RUNNING: stateStr = "RUNNING"; break;
        case VMState::PAUSED:  stateStr = "PAUSED";  break;
    }
    std::cout << "=== Virtual Machine: " << m_name << " [" << stateStr << "] ===\n";
}

} // namespace vmpm
