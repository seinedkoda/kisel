#pragma once

#include <QObject>
#include <QMenu>
#include <QSystemTrayIcon>

#include "process_manager.hpp"

namespace kisel {
class TrayIcon : public QObject {
    Q_OBJECT
public:
    explicit TrayIcon(ProcessManager* processManager);
private slots:
    void onRunningChanged(bool isRunning);
private:
    QSystemTrayIcon m_trayIcon;
    QMenu m_trayMenu;
};
}