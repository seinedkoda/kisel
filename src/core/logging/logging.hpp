#pragma once

#include <QCoreApplication>
#include <QtMessageHandler>

namespace kisel {
void logToFile(QtMsgType type, const QMessageLogContext& context, const QString& msg);
void logSystemInfo();
void setupLogging();
}