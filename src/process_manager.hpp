#pragma once

#include <QObject>
#include <QProcess>

#include "executable_file.hpp"

namespace kisel {
class ProcessManager : public QObject {
    Q_OBJECT

public:
    explicit ProcessManager(QObject* parent = nullptr);

    void run(const ExecutableFile& exeFile);
    void stop();
    [[nodiscard]] bool isRunning() const;

signals:
    void runningChanged(bool isRunning);
    void processError(const QString& errorText);

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    bool isReadyToRun(const ExecutableFile& exeFile);

    QProcess m_process;
    bool m_isRunning = false;
};
}