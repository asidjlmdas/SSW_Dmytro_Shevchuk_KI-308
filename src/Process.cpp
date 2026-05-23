#include "Process.h"
#include <stdexcept>

namespace vmpm {

Process::Process(int pid, const std::string& name, int parentPid)
    : m_pid(pid)
    , m_parentPid(parentPid)
    , m_name(name)
    , m_state(ProcessState::RUNNING)
    , m_priority(ProcessPriority::NORMAL)
    , m_stats{0.0, 0, 0}
    , m_startTime(std::chrono::steady_clock::now())
{}

int Process::getPid() const { return m_pid; }
int Process::getParentPid() const { return m_parentPid; }
const std::string& Process::getName() const { return m_name; }
ProcessState Process::getState() const { return m_state; }
ProcessPriority Process::getPriority() const { return m_priority; }

ProcessStats Process::getStats() const {
    ProcessStats s = m_stats;
    s.uptimeMs = getUptimeMs();
    return s;
}

std::string Process::getStateString() const {
    switch (m_state) {
        case ProcessState::RUNNING:   return "RUNNING";
        case ProcessState::SUSPENDED: return "SUSPENDED";
        case ProcessState::STOPPED:   return "STOPPED";
        case ProcessState::ZOMBIE:    return "ZOMBIE";
    }
    return "UNKNOWN";
}

bool Process::suspend() {
    if (m_state != ProcessState::RUNNING) return false;
    m_state = ProcessState::SUSPENDED;
    return true;
}

bool Process::resume() {
    if (m_state != ProcessState::SUSPENDED) return false;
    m_state = ProcessState::RUNNING;
    return true;
}

bool Process::stop() {
    if (m_state == ProcessState::STOPPED || m_state == ProcessState::ZOMBIE) return false;
    m_state = ProcessState::STOPPED;
    return true;
}

bool Process::kill() {
    m_state = ProcessState::ZOMBIE;
    return true;
}

void Process::setPriority(ProcessPriority priority) {
    m_priority = priority;
}

void Process::updateStats(double cpu, size_t memory) {
    m_stats.cpuUsage    = cpu;
    m_stats.memoryBytes = memory;
    m_stats.uptimeMs    = getUptimeMs();
}

bool Process::isAlive() const {
    return m_state == ProcessState::RUNNING || m_state == ProcessState::SUSPENDED;
}

bool Process::isZombie() const {
    return m_state == ProcessState::ZOMBIE;
}

uint64_t Process::getUptimeMs() const {
    auto now = std::chrono::steady_clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count()
    );
}

} // namespace vmpm
