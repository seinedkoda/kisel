#pragma once

#include <QObject>
#include <QProcess>

#include "executable_file.hpp"
#include "run_config.hpp"

namespace kisel {
class ProcessManager : public QObject {
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

    explicit ProcessManager(QObject* parent = nullptr);

    void run(const ExecutableFile& exeFile, RunConfig* runConfig);
    void runWineCfg(const Prefix* prefix);
    void runExplorer(const Prefix* prefix);
    void runRegedit(const Prefix* prefix);
    void runUninstaller(const Prefix* prefix);
    void stop();
    [[nodiscard]] bool isRunning() const;

signals:
    void runningChanged(bool isRunning);
    void runningError(kisel::ProcessManager::RunningError error, const QString& errorText = "");

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    bool preRunCheck(const ExecutableFile& exeFile, RunConfig* runConfig);
    void runWinetricksUtility(const QString& utilName, const Prefix* prefix);
    void showError(const QString& errorText, RunningError error, bool emitText = false);

    QProcess m_process;
    bool m_isRunning = false;
};
}