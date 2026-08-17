#include "tray_icon.hpp"
#include "run_manager.hpp"

#include <QAction>
#include <QCoreApplication>

using namespace kisel;

TrayIcon::TrayIcon(RunManager* runManager)
    : QObject(runManager)
    , m_trayIcon(new QSystemTrayIcon(QIcon(":/icons/kisel.svg"), this))
{
    auto* terminateAction = new QAction(tr("Terminate the process"), &m_trayMenu);
    connect(terminateAction, &QAction::triggered, this, [runManager]() { runManager->stop(); });
    m_trayMenu.addAction(terminateAction);

    m_trayMenu.addSeparator();

    auto* quitAction = new QAction(tr("Exit"), this);
    connect(quitAction, &QAction::triggered, this, [runManager]() {
        runManager->stop();
        qApp->quit();
    });
    m_trayMenu.addAction(quitAction);

    m_trayIcon->setContextMenu(&m_trayMenu);

    connect(runManager, &RunManager::runningChanged, this, &TrayIcon::onRunningChanged);
}

void TrayIcon::onRunningChanged(bool isRunning)
{
    if (isRunning) {
        m_trayIcon->show();
        m_trayIcon->setToolTip(tr("Kisel: %1").arg(RUN_MANAGER->taskName()));
    } else {
        m_trayIcon->setToolTip(tr("Kisel"));
        m_trayIcon->hide();
    }
}