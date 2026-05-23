#include "Logger.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace vmpm {

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::string prefix;
    switch (level) {
        case LogLevel::INFO:    prefix = "[INFO]   "; break;
        case LogLevel::WARNING: prefix = "[WARN]   "; break;
        case LogLevel::ERROR:   prefix = "[ERROR]  "; break;
    }
    std::string entry = currentTimestamp() + " " + prefix + message;
    m_entries.push_back(entry);
    std::cout << entry << "\n";
}

void Logger::info(const std::string& message)  { log(LogLevel::INFO,    message); }
void Logger::warn(const std::string& message)  { log(LogLevel::WARNING, message); }
void Logger::error(const std::string& message) { log(LogLevel::ERROR,   message); }

bool Logger::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (auto& e : m_entries) file << e << "\n";
    return true;
}

void Logger::clear() { m_entries.clear(); }

const std::vector<std::string>& Logger::getEntries() const { return m_entries; }

std::string Logger::currentTimestamp() const {
    auto now = std::time(nullptr);
    auto tm  = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

} // namespace vmpm
