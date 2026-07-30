#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QToolButton>

#include "executable_file.hpp"
#include "prefix.hpp"
#include "process_manager.hpp"

namespace kisel {
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(ProcessManager& processManager, const QString& exePath = "");

private slots:
    void onExeSelectionClicked();
    void onPlayStopButtonClicked();
    void onCreateExeShortcutButtonClicked();
    void onRunningError(ProcessManager::RunningError error, const QString& errorText);
    void onRunningChanged(bool isRunning);
    void onCurrentPrefixTextChanged(const QString& prefixName);
    void onCurrentCtIndexChanged(int index);

private:
    static void openPrefixWindow();
    static void openCtWindow();
    static void openAppSettingsWindow();
    void setExecutablePath(const QString& exePath);
    void setPreferredPrefix();
    void setPrefix(Prefix* prefix);

    const QPixmap m_unknownExePixmap { QIcon::fromTheme("unknown").pixmap(64, 64) };
    ProcessManager& m_processManager;
    ExecutableFile* m_exeFile = nullptr;
    QLabel* m_exeIconLabel;
    QLabel* m_exeNameLabel;
    QPushButton* m_playStopButton;
    QToolButton* m_createExeShortcutButton;
    QToolButton* m_exeSelectionButton;
    QComboBox* m_prefixComboBox;
    QAction* m_prefixSettingsAction;
    QAction* m_prefixComponentsAction;
    QAction* m_prefixOpenAction;
    QToolButton* m_prefixMenuButton;
    QComboBox* m_ctComboBox;
    QToolButton* m_ctWindowButton;
    QToolButton* m_appSettingsWindowButton;
};
};
