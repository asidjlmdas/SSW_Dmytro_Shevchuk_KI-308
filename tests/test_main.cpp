#include "ProcessManager.h"
#include "VirtualMachine.h"
#include <cassert>
#include <iostream>

using namespace vmpm;

void testProcessLifecycle() {
    Process p(1001, "test_proc", -1);
    assert(p.getState() == ProcessState::RUNNING);
    assert(p.isAlive());

    assert(p.suspend());
    assert(p.getState() == ProcessState::SUSPENDED);
    assert(!p.suspend()); // вже призупинений

    assert(p.resume());
    assert(p.getState() == ProcessState::RUNNING);

    assert(p.stop());
    assert(!p.isAlive());

    std::cout << "[PASS] testProcessLifecycle\n";
}

void testProcessManager() {
    ProcessManager pm;
    int pid1 = pm.startProcess("proc1");
    int pid2 = pm.startProcess("proc2", pid1);

    assert(pm.getAllProcesses().size() == 2);
    assert(pm.findByPid(pid1) != nullptr);
    assert(pm.findByName("proc1").size() == 1);
    assert(pm.getChildren(pid1).size() == 1);

    assert(pm.suspendProcess(pid1));
    assert(pm.resumeProcess(pid1));
    assert(pm.stopProcess(pid1));
    assert(!pm.stopProcess(pid1)); // вже зупинений

    assert(pm.killProcess(pid2));
    pm.killZombies();
    assert(pm.getAllProcesses().size() == 1); // pid2 видалено

    std::cout << "[PASS] testProcessManager\n";
}

void testPriority() {
    ProcessManager pm;
    int pid = pm.startProcess("prio_proc");
    assert(pm.setPriority(pid, ProcessPriority::HIGH));
    auto proc = pm.findByPid(pid);
    assert(proc->getPriority() == ProcessPriority::HIGH);
    std::cout << "[PASS] testPriority\n";
}

void testVirtualMachine() {
    VirtualMachine vm("TestVM");
    assert(vm.getState() == VMState::STOPPED);
    assert(vm.start());
    assert(vm.isRunning());
    assert(vm.pause());
    assert(vm.getState() == VMState::PAUSED);
    assert(vm.resume());
    assert(vm.stop());
    assert(vm.getState() == VMState::STOPPED);
    std::cout << "[PASS] testVirtualMachine\n";
}

void testLogger() {
    Logger::instance().clear();
    Logger::instance().info("Test message");
    assert(Logger::instance().getEntries().size() == 1);
    bool saved = Logger::instance().saveToFile("/tmp/test_log.txt");
    assert(saved);
    std::cout << "[PASS] testLogger\n";
}

int main() {
    std::cout << "=== Running Tests ===\n";
    testProcessLifecycle();
    testProcessManager();
    testPriority();
    testVirtualMachine();
    testLogger();
    std::cout << "=== All tests passed ===\n";
    return 0;
}
