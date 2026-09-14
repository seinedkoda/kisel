#include "tray_icon.hpp"

#include <QAction>
#include <QCoreApplication>

using namespace kisel;

TrayIcon::TrayIcon(RunManager* runManager)
    : QSystemTrayIcon(QIcon(":/icons/kisel.svg"), runManager)
    , m_runManager(runManager)
{
    connect(runManager, &RunManager::runningChanged, this, &TrayIcon::onRunningChanged);

    QAction* terminateAction = m_menu.addAction(tr("Terminate the process"));
    connect(terminateAction, &QAction::triggered, runManager, &RunManager::stop);

    m_menu.addSeparator();

    auto* quitAction = m_menu.addAction(tr("Exit"));
    connect(quitAction, &QAction::triggered, this, &TrayIcon::onQuitTriggered);

    setContextMenu(&m_menu);
}

void TrayIcon::onRunningChanged(bool isRunning)
{
    if (isRunning) {
        show();
        setToolTip(tr("Kisel: %1").arg(m_runManager->taskName()));
    } else {
        setToolTip(tr("Kisel"));
        hide();
    }
}

void TrayIcon::onQuitTriggered()
{
    m_runManager->stop();
    qApp->quit();
}