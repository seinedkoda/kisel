#include "logging.hpp"

#include <cstdio>

#include "core/appsettings/app_settings.hpp"

// NOLINTBEGIN

static FILE* g_logFile = nullptr;
static QtMessageHandler originalHandler = nullptr;

void kisel::logToFile(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString message = qFormatLogMessage(type, context, msg);

    if (g_logFile != nullptr) {
        fprintf(g_logFile, "%s\n", qPrintable(message));
        fflush(g_logFile);
    }

    if (originalHandler != nullptr) {
        originalHandler(type, context, msg);
    }
}

void kisel::logSystemInfo()
{
    qDebug() << "BUILD: kisel" << APP_VERSION << QSysInfo::buildCpuArchitecture();
    qDebug() << "FLATPAK:" << APP_SETTINGS->isFlatpak();
    qDebug() << "OS:" << QSysInfo::prettyProductName();
    qDebug() << "ARCH:" << QSysInfo::currentCpuArchitecture();
    qDebug() << "SYSTEM LOCALE:" << QLocale::system().name();
    qDebug() << "APPDATA:" << APP_SETTINGS->appDataDir().absolutePath();
    qDebug() << "ARGS:" << QCoreApplication::arguments() << '\n';
}

void kisel::setupLogging()
{
    const QString& logFilePath = APP_SETTINGS->logFilePath();
    g_logFile = fopen(qPrintable(logFilePath), "w");
    if (g_logFile == nullptr) {
        fprintf(stderr, "Failed to open the log file: %s\n", qPrintable(logFilePath));
        return;
    }

    qSetMessagePattern("[%{time hh:mm:ss}|%{type}]: %{message}");

    originalHandler = qInstallMessageHandler(logToFile);

    qDebug() << "=== START LOGGING" << QDateTime::currentDateTime().toString(Qt::ISODate) << "===";
    logSystemInfo();
}

// NOLINTEND