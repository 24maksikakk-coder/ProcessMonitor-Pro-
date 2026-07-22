// processmonitor_cpp.cpp — монитор процессов на C++

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

struct ProcessInfo {
    int pid;
    std::string name;
    long memory_mb;
    float cpu;
    int threads;
    std::string status;
};

class ProcessMonitor {
private:
    std::string sort_by;
    std::string filter_name;
    int interval;

    std::string read_file(const std::string& path) {
        std::ifstream file(path);
        std::string content;
        if (file.is_open()) {
            std::getline(file, content);
            file.close();
        }
        return content;
    }

    std::vector<ProcessInfo> get_processes() {
        std::vector<ProcessInfo> processes;
        DIR* dir = opendir("/proc");
        if (!dir) return processes;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type != DT_DIR) continue;
            int pid = atoi(entry->d_name);
            if (pid == 0) continue;

            // Чтение имени процесса
            std::string cmdline_path = "/proc/" + std::string(entry->d_name) + "/cmdline";
            std::ifstream cmdline(cmdline_path);
            std::string name;
            if (cmdline.is_open()) {
                std::getline(cmdline, name, '\0');
                cmdline.close();
                if (name.empty()) {
                    std::string stat_path = "/proc/" + std::string(entry->d_name) + "/stat";
                    std::string stat = read_file(stat_path);
                    size_t pos = stat.find('(');
                    if (pos != std::string::npos) {
                        size_t end = stat.find(')', pos);
                        if (end != std::string::npos) {
                            name = stat.substr(pos+1, end-pos-1);
                        }
                    }
                }
            }
            if (name.empty()) name = entry->d_name;

            // Фильтр
            if (!filter_name.empty() && name.find(filter_name) == std::string::npos) {
                continue;
            }

            // Чтение памяти из статуса
            std::string status_path = "/proc/" + std::string(entry->d_name) + "/status";
            std::ifstream status(status_path);
            long mem_kb = 0;
            std::string line;
            while (std::getline(status, line)) {
                if (line.rfind("VmRSS:", 0) == 0) {
                    std::istringstream iss(line);
                    std::string label;
                    iss >> label >> mem_kb;
                    break;
                }
            }
            status.close();

            // Чтение CPU (упрощённо)
            float cpu = 0.0;
            std::string stat_path = "/proc/" + std::string(entry->d_name) + "/stat";
            std::ifstream stat_file(stat_path);
            if (stat_file.is_open()) {
                std::string stat_line;
                std::getline(stat_file, stat_line);
                std::istringstream iss(stat_line);
                std::string token;
                int field = 0;
                long utime = 0, stime = 0;
                while (iss >> token) {
                    if (field == 13) utime = std::stol(token);
                    if (field == 14) stime = std::stol(token);
                    field++;
                }
                // CPU = (utime + stime) / total_time (упрощённо)
                stat_file.close();
            }

            // Чтение потоков и статуса
            int threads = 0;
            std::string status_line = read_file(status_path);
            std::istringstream status_iss(status_line);
            while (std::getline(status_iss, line)) {
                if (line.rfind("Threads:", 0) == 0) {
                    std::istringstream iss(line);
                    std::string label;
                    iss >> label >> threads;
                    break;
                }
                if (line.rfind("State:", 0) == 0) {
                    std::istringstream iss(line);
                    std::string label, state;
                    iss >> label >> state;
                    // state - первый символ статуса (R, S, D, Z, T)
                }
            }

            ProcessInfo info;
            info.pid = pid;
            info.name = name;
            info.memory_mb = mem_kb / 1024;
            info.cpu = cpu;
            info.threads = threads;
            info.status = "running";
            processes.push_back(info);
        }
        closedir(dir);
        return processes;
    }

public:
    ProcessMonitor(const std::string& sort = "memory", const std::string& filter = "", int interval = 2)
        : sort_by(sort), filter_name(filter), interval(interval) {}

    void run() {
        std::cout << "\033[2J\033[H"; // очистка экрана
        std::cout << "📊 ProcessMonitor Pro — C++ Edition" << std::endl;
        std::cout << "Нажмите Ctrl+C для выхода" << std::endl << std::endl;

        while (true) {
            auto processes = get_processes();
            // Сортировка
            if (sort_by == "memory") {
                std::sort(processes.begin(), processes.end(),
                          [](const ProcessInfo& a, const ProcessInfo& b) {
                              return a.memory_mb > b.memory_mb;
                          });
            } else if (sort_by == "cpu") {
                std::sort(processes.begin(), processes.end(),
                          [](const ProcessInfo& a, const ProcessInfo& b) {
                              return a.cpu > b.cpu;
                          });
            } else if (sort_by == "pid") {
                std::sort(processes.begin(), processes.end(),
                          [](const ProcessInfo& a, const ProcessInfo& b) {
                              return a.pid < b.pid;
                          });
            } else {
                std::sort(processes.begin(), processes.end(),
                          [](const ProcessInfo& a, const ProcessInfo& b) {
                              return a.name < b.name;
                          });
            }

            // Вывод
            std::cout << "\033[2J\033[H";
            std::cout << "📊 ProcessMonitor Pro — C++ Edition" << std::endl;
            std::cout << "Процессов: " << processes.size()
                      << " | Фильтр: " << (filter_name.empty() ? "все" : filter_name)
                      << " | Сортировка: " << sort_by << std::endl;
            std::cout << std::string(90, '-') << std::endl;
            std::cout << std::setw(6) << "PID" << std::setw(20) << "ИМЯ"
                      << std::setw(12) << "ПАМЯТЬ" << std::setw(8) << "CPU"
                      << std::setw(8) << "ПОТОКИ" << std::setw(12) << "СТАТУС" << std::endl;
            std::cout << std::string(90, '-') << std::endl;

            int count = 0;
            for (const auto& p : processes) {
                if (count++ > 30) break;
                std::string color = "\033[92m";
                if (p.memory_mb > 500) color = "\033[91m";
                else if (p.memory_mb > 200) color = "\033[93m";
                std::cout << std::setw(6) << p.pid
                          << " " << std::setw(19) << p.name.substr(0, 19)
                          << " " << color << std::setw(10) << p.memory_mb << " MB\033[0m"
                          << " " << std::setw(6) << p.cpu << "%"
                          << " " << std::setw(8) << p.threads
                          << " " << std::setw(12) << p.status << std::endl;
            }
            std::cout << std::string(90, '-') << std::endl;
            long total_memory = 0;
            for (const auto& p : processes) total_memory += p.memory_mb;
            std::cout << "Всего памяти: " << total_memory << " MB" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(interval));
        }
    }
};

int main(int argc, char* argv[]) {
    std::string sort_by = "memory";
    std::string filter_name = "";
    int interval = 2;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sort" && i+1 < argc) {
            sort_by = argv[++i];
        } else if (arg == "--filter" && i+1 < argc) {
            filter_name = argv[++i];
        } else if (arg == "--interval" && i+1 < argc) {
            interval = std::stoi(argv[++i]);
        }
    }

    ProcessMonitor monitor(sort_by, filter_name, interval);
    monitor.run();
    return 0;
}
