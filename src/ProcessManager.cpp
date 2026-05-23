#include "ProcessManager.h"
#include "Logger.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>

namespace vmpm {

ProcessManager::ProcessManager() : m_nextPid(1000) {}

int ProcessManager::startProcess(const std::string& name, int parentPid) {
    int pid = m_nextPid++;
    auto proc = std::make_shared<Process>(pid, name, parentPid);
    m_processes.push_back(proc);

    std::string msg = "Started process '" + name + "' PID=" + std::to_string(pid);
    addLogEntry(msg);
    Logger::instance().info(msg);
    return pid;
}

bool ProcessManager::stopProcess(int pid) {
    auto proc = findProcess(pid);
    if (!proc) return false;
    bool ok = proc->stop();
    if (ok) {
        std::string msg = "Stopped process PID=" + std::to_string(pid);
        addLogEntry(msg);
        Logger::instance().info(msg);
    }
    return ok;
}

bool ProcessManager::killProcess(int pid) {
    auto proc = findProcess(pid);
    if (!proc) return false;
    proc->kill();
    std::string msg = "Killed process PID=" + std::to_string(pid);
    addLogEntry(msg);
    Logger::instance().warn(msg);
    return true;
}

bool ProcessManager::suspendProcess(int pid) {
    auto proc = findProcess(pid);
    if (!proc) return false;
    bool ok = proc->suspend();
    if (ok) {
        std::string msg = "Suspended process PID=" + std::to_string(pid);
        addLogEntry(msg);
        Logger::instance().info(msg);
    }
    return ok;
}

bool ProcessManager::resumeProcess(int pid) {
    auto proc = findProcess(pid);
    if (!proc) return false;
    bool ok = proc->resume();
    if (ok) {
        std::string msg = "Resumed process PID=" + std::to_string(pid);
        addLogEntry(msg);
        Logger::instance().info(msg);
    }
    return ok;
}

std::shared_ptr<Process> ProcessManager::findByPid(int pid) const {
    return findProcess(pid);
}

std::vector<std::shared_ptr<Process>> ProcessManager::findByName(const std::string& name) const {
    std::vector<std::shared_ptr<Process>> result;
    for (auto& p : m_processes) {
        if (p->getName() == name) result.push_back(p);
    }
    return result;
}

std::vector<std::shared_ptr<Process>> ProcessManager::getAllProcesses() const {
    return m_processes;
}

std::vector<std::shared_ptr<Process>> ProcessManager::getChildren(int parentPid) const {
    std::vector<std::shared_ptr<Process>> result;
    for (auto& p : m_processes) {
        if (p->getParentPid() == parentPid) result.push_back(p);
    }
    return result;
}

bool ProcessManager::setPriority(int pid, ProcessPriority priority) {
    auto proc = findProcess(pid);
    if (!proc) return false;
    proc->setPriority(priority);
    std::string msg = "Changed priority for PID=" + std::to_string(pid);
    addLogEntry(msg);
    Logger::instance().info(msg);
    return true;
}

void ProcessManager::updateStats() {
    // Симуляція статистики (у реальній системі — системні виклики)
    for (auto& p : m_processes) {
        if (p->isAlive()) {
            double cpu    = static_cast<double>(rand() % 300) / 10.0; // 0..30%
            size_t memory = (rand() % 512 + 64) * 1024 * 1024;        // 64..576 MB
            p->updateStats(cpu, memory);
        }
    }
}

void ProcessManager::killZombies() {
    m_processes.erase(
        std::remove_if(m_processes.begin(), m_processes.end(),
            [](const std::shared_ptr<Process>& p) { return p->isZombie(); }),
        m_processes.end()
    );
    addLogEntry("Zombie processes cleaned up");
    Logger::instance().info("Zombie processes cleaned up");
}

void ProcessManager::killByTimeout(uint64_t maxUptimeMs) {
    for (auto& p : m_processes) {
        if (p->isAlive() && p->getUptimeMs() > maxUptimeMs) {
            p->kill();
            std::string msg = "Killed timed-out process PID=" + std::to_string(p->getPid());
            addLogEntry(msg);
            Logger::instance().warn(msg);
        }
    }
}

void ProcessManager::printProcessList() const {
    std::cout << std::left
              << std::setw(8)  << "PID"
              << std::setw(20) << "Name"
              << std::setw(12) << "State"
              << std::setw(10) << "CPU%"
              << std::setw(12) << "Memory(MB)"
              << std::setw(10) << "Priority"
              << "\n";
    std::cout << std::string(72, '-') << "\n";

    for (auto& p : m_processes) {
        auto stats = p->getStats();
        std::string prio;
        switch (p->getPriority()) {
            case ProcessPriority::LOW:      prio = "LOW";      break;
            case ProcessPriority::NORMAL:   prio = "NORMAL";   break;
            case ProcessPriority::HIGH:     prio = "HIGH";     break;
            case ProcessPriority::REALTIME: prio = "REALTIME"; break;
        }
        std::cout << std::left
                  << std::setw(8)  << p->getPid()
                  << std::setw(20) << p->getName()
                  << std::setw(12) << p->getStateString()
                  << std::setw(10) << std::fixed << std::setprecision(1) << stats.cpuUsage
                  << std::setw(12) << (stats.memoryBytes / 1024 / 1024)
                  << std::setw(10) << prio
                  << "\n";
    }
}

void ProcessManager::printProcessTree(int rootPid, int indent) const {
    std::vector<std::shared_ptr<Process>> roots;

    if (rootPid == -1) {
        // Знайти кореневі процеси (без батька)
        for (auto& p : m_processes) {
            bool hasParent = false;
            for (auto& q : m_processes) {
                if (q->getPid() == p->getParentPid()) { hasParent = true; break; }
            }
            if (!hasParent) roots.push_back(p);
        }
    } else {
        auto root = findProcess(rootPid);
        if (root) roots.push_back(root);
    }

    std::function<void(std::shared_ptr<Process>, int)> printNode =
        [&](std::shared_ptr<Process> proc, int depth) {
            std::cout << std::string(depth * 2, ' ')
                      << (depth > 0 ? "|- " : "")
                      << "[" << proc->getPid() << "] "
                      << proc->getName()
                      << " (" << proc->getStateString() << ")\n";
            auto children = getChildren(proc->getPid());
            for (auto& child : children) printNode(child, depth + 1);
        };

    for (auto& r : roots) printNode(r, indent);
}

bool ProcessManager::saveLogsToFile(const std::string& filename) const {
    return Logger::instance().saveToFile(filename);
}

std::shared_ptr<Process> ProcessManager::findProcess(int pid) const {
    for (auto& p : m_processes) {
        if (p->getPid() == pid) return p;
    }
    return nullptr;
}

void ProcessManager::addLogEntry(const std::string& message) {
    m_log.push_back(message);
}

} // namespace vmpm
