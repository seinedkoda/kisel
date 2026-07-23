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
    void onProcessError(const QString& errorText);
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
    QToolButton* m_exeSelectionButton;
    QPushButton* m_playStopButton;
    QComboBox* m_prefixComboBox;
    QComboBox* m_ctComboBox;
    QToolButton* m_ctWindowButton;
    QLabel* m_exeIconLabel;
    QLabel* m_exeNameLabel;
    QToolButton* m_prefixWindowButton;
    QToolButton* m_settingsWindowButton;
    QToolButton* m_createExeShortcutButton;
    QCheckBox* m_mangohudCheckBox;
    QCheckBox* m_obsVkCaptureCheckBox;
    QCheckBox* m_waylandCheckBox;
    QCheckBox* m_useSteamCheckBox;
    QCheckBox* m_xaliaCheckBox;
    QCheckBox* m_wow64CheckBox;
};
};
