#include "VirtualMachine.h"
#include <iostream>
#include <string>
#include <limits>

using namespace vmpm;

void printMenu() {
    std::cout << "\n========== VM Process Manager ==========\n";
    std::cout << " 1. Запустити новий процес\n";
    std::cout << " 2. Зупинити процес (stop)\n";
    std::cout << " 3. Примусово завершити процес (kill)\n";
    std::cout << " 4. Призупинити процес\n";
    std::cout << " 5. Відновити процес\n";
    std::cout << " 6. Переглянути список процесів\n";
    std::cout << " 7. Показати дерево процесів\n";
    std::cout << " 8. Знайти процес за іменем\n";
    std::cout << " 9. Змінити пріоритет процесу\n";
    std::cout << "10. Оновити статистику (CPU/RAM)\n";
    std::cout << "11. Завершити зависші процеси (timeout)\n";
    std::cout << "12. Очистити zombie-процеси\n";
    std::cout << "13. Зберегти логи у файл\n";
    std::cout << " 0. Вийти\n";
    std::cout << "=========================================\n";
    std::cout << "Виберіть опцію: ";
}

int main() {
    std::srand(42);

    VirtualMachine vm("TestVM-1");
    vm.start();
    vm.printStatus();

    ProcessManager& pm = vm.getProcessManager();

    // Стартуємо кілька процесів для демонстрації
    int initPid   = pm.startProcess("init");
    int shellPid  = pm.startProcess("bash",    initPid);
    int serverPid = pm.startProcess("nginx",   initPid);
    int workerPid = pm.startProcess("worker1", serverPid);
    pm.updateStats();

    int choice;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                std::string name;
                int parentPid;
                std::cout << "Ім'я процесу: ";
                std::getline(std::cin, name);
                std::cout << "PID батька (-1 якщо немає): ";
                std::cin >> parentPid;
                std::cin.ignore();
                int newPid = pm.startProcess(name, parentPid);
                std::cout << "Процес запущено, PID=" << newPid << "\n";
                break;
            }
            case 2: {
                int pid;
                std::cout << "PID процесу: ";
                std::cin >> pid; std::cin.ignore();
                std::cout << (pm.stopProcess(pid) ? "Зупинено\n" : "Помилка\n");
                break;
            }
            case 3: {
                int pid;
                std::cout << "PID процесу: ";
                std::cin >> pid; std::cin.ignore();
                std::cout << (pm.killProcess(pid) ? "Завершено\n" : "Помилка\n");
                break;
            }
            case 4: {
                int pid;
                std::cout << "PID процесу: ";
                std::cin >> pid; std::cin.ignore();
                std::cout << (pm.suspendProcess(pid) ? "Призупинено\n" : "Помилка\n");
                break;
            }
            case 5: {
                int pid;
                std::cout << "PID процесу: ";
                std::cin >> pid; std::cin.ignore();
                std::cout << (pm.resumeProcess(pid) ? "Відновлено\n" : "Помилка\n");
                break;
            }
            case 6:
                pm.printProcessList();
                break;
            case 7:
                std::cout << "\n--- Дерево процесів ---\n";
                pm.printProcessTree();
                break;
            case 8: {
                std::string name;
                std::cout << "Ім'я процесу: ";
                std::getline(std::cin, name);
                auto found = pm.findByName(name);
                if (found.empty()) {
                    std::cout << "Не знайдено\n";
                } else {
                    for (auto& p : found)
                        std::cout << "PID=" << p->getPid()
                                  << " Name=" << p->getName()
                                  << " State=" << p->getStateString() << "\n";
                }
                break;
            }
            case 9: {
                int pid, pr;
                std::cout << "PID: "; std::cin >> pid; std::cin.ignore();
                std::cout << "Пріоритет (0=LOW 1=NORMAL 2=HIGH 3=REALTIME): ";
                std::cin >> pr; std::cin.ignore();
                ProcessPriority prio = static_cast<ProcessPriority>(pr);
                std::cout << (pm.setPriority(pid, prio) ? "Змінено\n" : "Помилка\n");
                break;
            }
            case 10:
                pm.updateStats();
                std::cout << "Статистику оновлено\n";
                break;
            case 11: {
                uint64_t ms;
                std::cout << "Макс. час роботи (мс): ";
                std::cin >> ms; std::cin.ignore();
                pm.killByTimeout(ms);
                std::cout << "Завершено зависші процеси\n";
                break;
            }
            case 12:
                pm.killZombies();
                std::cout << "Zombie-процеси очищено\n";
                break;
            case 13: {
                std::string filename;
                std::cout << "Ім'я файлу: ";
                std::getline(std::cin, filename);
                std::cout << (pm.saveLogsToFile(filename) ? "Збережено\n" : "Помилка запису\n");
                break;
            }
            default:
                std::cout << "Невірна опція\n";
        }
    }

    vm.stop();
    std::cout << "До побачення!\n";
    return 0;
}
