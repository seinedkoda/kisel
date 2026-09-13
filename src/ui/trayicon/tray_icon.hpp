#pragma once

#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

#include "core/run/run_manager.hpp"

namespace kisel {
class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit TrayIcon(RunManager* runManager);

private slots:
    void onRunningChanged(bool isRunning);
    void onQuitTriggered();

private:
    QMenu m_menu;
    RunManager* m_runManager;
};
}