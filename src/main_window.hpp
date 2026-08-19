#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QToolButton>

#include "run_manager.hpp"

namespace kisel {
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString& exePath = "");

private slots:
    void onExeSelectionClicked();
    void onRunStopTriggered();
    void onCreateShortcutTriggered();
    void onRunningError(kisel::RunManager::RunningError error, const QString& errorText);
    void onRunningChanged(bool isRunning);
    void onCurrentPrefixTextChanged(const QString& prefixName);
    void onCurrentCtIndexChanged(int index);
    void individualPrefixStateChanged(bool checked);
    void openLogFile();

private:
    static void openPrefixWindow();
    static void openCtWindow();
    static void openAppSettingsWindow();
    void setExecutablePath(const QString& exePath);
    void setPreferredPrefix();
    void setPrefix(Prefix* prefix);
    void setPreferredCt();

    const QSize m_exeIconSize { 64, 64 };
    const QPixmap m_unknownExePixmap { QIcon::fromTheme("unknown").pixmap(m_exeIconSize) };
    QString m_lastSearchPath = QDir::homePath();
    bool m_manuallyCheckedIndividual = false;
    RunConfig* m_runConfig;
    QString m_individualPrefixName;
    QLabel* m_exeIconLabel;
    QLabel* m_exeNameLabel;
    QAction* m_runStopAction;
    QMenu* m_prefixToolsMenu;
    QToolButton* m_runStopButton;
    QToolButton* m_exeSelectionButton;
    QPointer<Prefix> m_individualPrefix;
    QCheckBox* m_individualPrefixCheckBox;
    QComboBox* m_prefixComboBox;
    QAction* m_prefixSettingsAction;
    QAction* m_prefixComponentsAction;
    QAction* m_prefixOpenAction;
    QToolButton* m_prefixMenuButton;
    QComboBox* m_ctComboBox;
    QToolButton* m_ctWindowButton;
};
};
