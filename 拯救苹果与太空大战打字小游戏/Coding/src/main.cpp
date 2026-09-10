#include "mainwindow.h"
#include "core/logger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("TypingHero");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("TypingHero");

    LOG_INFO("Application starting...");
    LOG_INFO(QString("Application version: %1").arg(a.applicationVersion()));
    LOG_INFO(QString("Log file: %1").arg(Logger::instance().getLogFilePath()));

    MainWindow w;
    w.show();

    int result = a.exec();
    
    LOG_INFO(QString("Application exiting with code: %1").arg(result));
    
    return result;
}
