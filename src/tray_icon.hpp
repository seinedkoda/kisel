#pragma once

#include <QMenu>
#include <QObject>
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
    QMenu m_trayMenu;
    QSystemTrayIcon* m_trayIcon;
};
}