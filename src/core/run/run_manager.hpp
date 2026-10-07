#pragma once

#include <QProcess>

#include "core/compatibilitytools/ct_model.hpp"
#include "core/prefix/prefix_model.hpp"
#include "run_config.hpp"

namespace kisel {
using WinetricksCallback = const std::function<void(int, QProcess::ExitStatus, const QString& output)>&;

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
        UnknownError,
    };
    Q_ENUM(RunningError)

    explicit RunManager(PrefixModel* prefixModel, CtModel* ctModel, QObject* parent = nullptr);

    void runExe(RunConfig* config);
    void runWineCfg(Prefix* prefix);
    void runExplorer(Prefix* prefix);
    void runRegedit(Prefix* prefix);
    void runUninstaller(Prefix* prefix);
    void runComponentsList(Prefix* prefix, const QString& category, WinetricksCallback callback);
    void runInstalledComponentsList(Prefix* prefix, const QString& category, WinetricksCallback callback);
    void runComponentsInstallation(Prefix* prefix, const QStringList& components, WinetricksCallback callback);
    void stop();
    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] QString taskName() const;

signals:
    void runningChanged(bool isRunning, bool isExe = false);
    void exeRunningError(kisel::RunManager::RunningError error, const QString& errorText = "");

private slots:
    void onExeProcessStarted();
    void onExeProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onExeProcessError(QProcess::ProcessError error);

private:
    bool setupPrefix(RunConfig* config);
    bool setupCt(RunConfig* config);
    bool setupProcess(RunConfig* config, const QStringList& runnerCommandParts, QProcessEnvironment& env, const QStringList& args = { });
    static QStringList setupProtonCommand(RunConfig* config, QProcessEnvironment& env);
    QStringList setupUmuCommand(RunConfig* config, QProcessEnvironment& env);
    void setupExeProcessLogging(RunConfig* config);
    void runWinetricks(Prefix* prefix, const QStringList& args, const QString& taskName = { }, WinetricksCallback callback = { });
    void showError(const QString& errorText, RunningError error, bool emitText = false);

    QString m_currentTaskName;
    QString m_fullCommandString;
    bool m_isRunning;
    QProcess* m_process;
    PrefixModel* m_prefixModel;
    CtModel* m_ctModel;
};
}