#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace vmpm {

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static Logger& instance();

    void log(LogLevel level, const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

    bool saveToFile(const std::string& filename) const;
    void clear();
    const std::vector<std::string>& getEntries() const;

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::vector<std::string> m_entries;
    std::string currentTimestamp() const;
};

} // namespace vmpm
