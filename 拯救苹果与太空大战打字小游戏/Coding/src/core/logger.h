#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& instance();
    
    void log(LogLevel level, const QString& message, const QString& file = "", int line = 0, const QString& function = "");
    void debug(const QString& message, const QString& file = "", int line = 0, const QString& function = "");
    void info(const QString& message, const QString& file = "", int line = 0, const QString& function = "");
    void warning(const QString& message, const QString& file = "", int line = 0, const QString& function = "");
    void error(const QString& message, const QString& file = "", int line = 0, const QString& function = "");
    void fatal(const QString& message, const QString& file = "", int line = 0, const QString& function = "");
    
    void setLogFile(const QString& filePath);
    void setConsoleOutput(bool enabled);
    void setFileOutput(bool enabled);
    void setMinLevel(LogLevel level);
    
    QString getLogFilePath() const;
    
private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    QString levelToString(LogLevel level) const;
    QString formatMessage(LogLevel level, const QString& message, const QString& file, int line, const QString& function) const;
    
    QFile m_logFile;
    QTextStream m_textStream;
    QMutex m_mutex;
    bool m_consoleOutput;
    bool m_fileOutput;
    LogLevel m_minLevel;
};

#define LOG_DEBUG(msg) Logger::instance().debug(msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO(msg) Logger::instance().info(msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARNING(msg) Logger::instance().warning(msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR(msg) Logger::instance().error(msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_FATAL(msg) Logger::instance().fatal(msg, __FILE__, __LINE__, __FUNCTION__)

#endif // LOGGER_H
