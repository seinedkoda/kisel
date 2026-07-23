#include "tray_icon.hpp"

#include <QCoreApplication>
#include <QAction>

using namespace kisel;

TrayIcon::TrayIcon(ProcessManager* processManager)
    : QObject(processManager)
{
    m_trayIcon.setIcon(QIcon(":/icons/kisel.svg"));

    auto* terminateAction = new QAction(tr("Terminate the process"), &m_trayMenu);
    connect(terminateAction, &QAction::triggered, this, [processManager]() { processManager->stop(); });
    m_trayMenu.addAction(terminateAction);

    m_trayMenu.addSeparator();

    auto* quitAction = new QAction(tr("Exit"), this);
    connect(quitAction, &QAction::triggered, this, [processManager]() {
        processManager->stop();
        qApp->quit();
    });
    m_trayMenu.addAction(quitAction);

    m_trayIcon.setContextMenu(&m_trayMenu);

    connect(processManager, &ProcessManager::runningChanged, this, &TrayIcon::onRunningChanged);
}

void TrayIcon::onRunningChanged(bool isRunning)
{
    if (isRunning) {
        m_trayIcon.show();
    } else {
        m_trayIcon.hide();
    }
}