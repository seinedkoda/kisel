#pragma once

#include <QProcess>

#include "run_config.hpp"

namespace kisel {
#define RUN_MANAGER RunManager::instance()

class RunManager : public QObject {
    Q_OBJECT

public:
    enum RunningError {
        AlreadyRunning,
        InvalidExecutable,
        PrefixWriteError,
        InvalidCt,
        NoUmu,
        NoWinetricks,
        FailedToStart,
        Crashed,
        Timedout,
        ReadError,
        WriteError,
        UnknownError
    };
    Q_ENUM(RunningError)

    static RunManager* instance();

    void run(RunConfig* runConfig);
    void runWineCfg(const Prefix* prefix);
    void runExplorer(const Prefix* prefix);
    void runRegedit(const Prefix* prefix);
    void runUninstaller(const Prefix* prefix);
    void stop();
    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] QString taskName() const;

signals:
    void runningChanged(bool isRunning);
    void runningError(kisel::RunManager::RunningError error, const QString& errorText = "");

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    explicit RunManager(QObject* parent = nullptr);

    bool setupConfig(RunConfig* runConfig);
    bool setupPrefix();
    bool setupCt();
    void setupProtonProcess();
    void setupUmuProcess();
    void setupLogging();
    void runWinetricksUtility(const Prefix* prefix, const QString& utilName);
    void showError(const QString& errorText, RunningError error, bool emitText = false);

    QProcess m_process;
    bool m_isRunning = false;
    RunConfig* m_runConfig;
    QString m_currentTaskName;
};
}