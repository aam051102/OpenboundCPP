#include "Logger.h"
#include "Common.h"

#include <ctime>
#include <sstream>
#include <filesystem>

const char *LevelToString(SBURB::Logger::LogLevel level)
{
    switch (level)
    {
    case SBURB::Logger::Info:
        return "INFO";
    case SBURB::Logger::Error:
        return "ERROR";
    case SBURB::Logger::Warning:
        return "WARN";
    // case SBURB::Logger::Debug:
    default:
        return "DEBUG";
    }
}

namespace SBURB
{
    static Logger globalLogger{};

    Logger::Logger()
    {
        
    }

    Logger::~Logger()
    {
    }

    void Logger::OpenLogFile() {
        if (!logFile.is_open()) {
            std::string appdataDir = std::getenv("APPDATA");
            std::string baseDir = appdataDir;

            if (appdataDir == "")
                baseDir = GetExecutableDirectory();

            // Define path
            std::filesystem::path filePath(baseDir);
            if (appdataDir != "")
                filePath /= "OpenboundCPP";

            filePath /= "logs";

            // Create directories
            std::filesystem::create_directories(filePath);

            // Append filename
            auto duration = std::chrono::system_clock::now().time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

            filePath /= std::to_string(millis) + ".txt";

            logFile.open(filePath.string(), std::ios::app);
        }
    }

    Logger *Logger::GetGlobalLogger()
    {
        return &globalLogger;
    }

    void Logger::_unique_Log(LogLevel level, std::string message, const char *calling, const char *file, int line)
    {
        this->OpenLogFile();

        std::time_t t = std::time(0);
        std::tm *now = std::localtime(&t);
        
        std::stringstream out;
        // "2019-7-7@15:55 [INFO | main] This is a test log"
        out << now->tm_year + 1900 << '-'
            << (now->tm_mon + 1) << '-'
            << now->tm_mday
            << "@"
            << now->tm_hour << ":"
            << now->tm_min
            << " [" << LevelToString(level) << " | " << calling << "@" << file << ":" << line << "] "
            << message << "\n";
        std::cout << out.str();
        logFile << out.str();
    }
}