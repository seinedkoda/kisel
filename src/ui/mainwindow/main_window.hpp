#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QToolButton>

#include "core/run/run_manager.hpp"

namespace kisel {
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(RunConfig* config = nullptr);

private slots:
    static void onOpenPrefixListWidget();
    static void onOpenCtListWidget();
    void onRunStopTriggered();
    void onExeSelectionClicked();
    void onEditShortcutsTriggered();
    void onRunningError(kisel::RunManager::RunningError error, const QString& errorText);
    void onRunningChanged(bool isRunning, bool isExe);
    void onPrefixTextSelected(const QString& prefixName);
    void onCurrentCtIndexChanged(int index);
    void onPrefixTypeSelected(int index);

private:
    static void openAppSettingsWindow();
    void setExecutablePath(const QString& exePath);
    void setPreferredPrefix();
    void newIndividualPrefixFromExe();
    void newPortablePrefixFromExe();
    void setPrefix(Prefix* prefix);
    void setSharedPrefix(Prefix* prefix);
    void setIndividualPrefix();
    void setPortablePrefix();
    void setPreferredCt();

    const QSize m_exeIconSize { 64, 64 };
    const QPixmap m_unknownExePixmap { QIcon::fromTheme("unknown").pixmap(m_exeIconSize) };
    QString m_lastSearchPath = QDir::homePath();
    RunConfig* m_runConfig;
    QString m_individualPrefixName;
    QLabel* m_exeIconLabel;
    QLabel* m_exeNameLabel;
    QAction* m_runStopAction;
    QMenu* m_prefixToolsMenu;
    QToolButton* m_runStopButton;
    QToolButton* m_exeSelectionButton;
    QPointer<Prefix> m_individualPrefix;
    QPointer<Prefix> m_portablePrefix;
    QComboBox* m_prefixTypeComboBox;
    QComboBox* m_prefixComboBox;
    QAction* m_prefixSettingsAction;
    QAction* m_prefixComponentsAction;
    QAction* m_prefixOpenAction;
    QToolButton* m_prefixMenuButton;
    QComboBox* m_ctComboBox;
    QToolButton* m_ctWindowButton;
};
};
