#include "logger.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <iostream>

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

Logger::Logger()
    : m_consoleOutput(true)
    , m_fileOutput(true)
    , m_minLevel(LogLevel::Debug) {
    
    QString logDir = QCoreApplication::applicationDirPath() + "/logs";
    QDir dir;
    if (!dir.exists(logDir)) {
        dir.mkpath(logDir);
    }
    
    QString logFileName = QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".log";
    QString logFilePath = logDir + "/" + logFileName;
    
    setLogFile(logFilePath);
}

Logger::~Logger() {
    if (m_logFile.isOpen()) {
        m_textStream.flush();
        m_logFile.close();
    }
}

void Logger::setLogFile(const QString& filePath) {
    QMutexLocker locker(&m_mutex);
    
    if (m_logFile.isOpen()) {
        m_textStream.flush();
        m_logFile.close();
    }
    
    m_logFile.setFileName(filePath);
    
    if (m_fileOutput) {
        if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Failed to open log file:" << filePath;
        } else {
            m_textStream.setDevice(&m_logFile);
        }
    }
}

void Logger::setConsoleOutput(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_consoleOutput = enabled;
}

void Logger::setFileOutput(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_fileOutput = enabled;
    
    if (m_fileOutput && !m_logFile.isOpen() && !m_logFile.fileName().isEmpty()) {
        if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Failed to open log file:" << m_logFile.fileName();
        } else {
            m_textStream.setDevice(&m_logFile);
        }
    } else if (!m_fileOutput && m_logFile.isOpen()) {
        m_textStream.flush();
        m_logFile.close();
    }
}

void Logger::setMinLevel(LogLevel level) {
    QMutexLocker locker(&m_mutex);
    m_minLevel = level;
}

QString Logger::getLogFilePath() const {
    return m_logFile.fileName();
}

void Logger::log(LogLevel level, const QString& message, const QString& file, int line, const QString& function) {
    if (level < m_minLevel) {
        return;
    }
    
    QString formattedMessage = formatMessage(level, message, file, line, function);
    
    QMutexLocker locker(&m_mutex);
    
    if (m_consoleOutput) {
        switch (level) {
            case LogLevel::Debug:
                qDebug() << formattedMessage;
                break;
            case LogLevel::Info:
                qInfo() << formattedMessage;
                break;
            case LogLevel::Warning:
                qWarning() << formattedMessage;
                break;
            case LogLevel::Error:
                qCritical() << formattedMessage;
                break;
            case LogLevel::Fatal:
                qFatal("%s", qPrintable(formattedMessage));
                break;
        }
    }
    
    if (m_fileOutput && m_logFile.isOpen()) {
        m_textStream << formattedMessage << "\n";
        m_textStream.flush();
    }
}

void Logger::debug(const QString& message, const QString& file, int line, const QString& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const QString& message, const QString& file, int line, const QString& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const QString& message, const QString& file, int line, const QString& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const QString& message, const QString& file, int line, const QString& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::fatal(const QString& message, const QString& file, int line, const QString& function) {
    log(LogLevel::Fatal, message, file, line, function);
}

QString Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Fatal:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

QString Logger::formatMessage(LogLevel level, const QString& message, const QString& file, int line, const QString& function) const {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    
    QString location;
    if (!file.isEmpty() && line > 0) {
        QString fileName = QFileInfo(file).fileName();
        location = QString("[%1:%2]").arg(fileName).arg(line);
        if (!function.isEmpty()) {
            location += QString("[%1]").arg(function);
        }
    }
    
    return QString("[%1] [%2] %3 %4").arg(timestamp, levelStr, location, message);
}
