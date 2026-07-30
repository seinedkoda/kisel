#include "process_manager.hpp"

#include <QStandardPaths>

#include "app_settings.hpp"
#include "executable_file.hpp"
#include "translator.hpp"

using namespace kisel;

ProcessManager::ProcessManager(QObject* parent)
    : QObject(parent)
{
    connect(&m_process, &QProcess::started, this, &ProcessManager::onProcessStarted);
    connect(&m_process, &QProcess::finished, this, &ProcessManager::onProcessFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &ProcessManager::onProcessError);
}

void ProcessManager::run(const ExecutableFile& exeFile)
{
    if (!isReadyToRunExecutable(exeFile)) {
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const Prefix* prefix = exeFile.prefix();
    const PrefixSettings* settings = prefix->settings();
    const Ct* ct = prefix->ct();
    bool useSteam = APP_SETTINGS->steamExists() && settings->steamEnabled();

    env.insert("WINEPREFIX", prefix->path());
    env.insert("MANGOHUD", settings->mangoHudEnabled() ? "1" : "0");
    env.insert("OBS_VKCAPTURE", settings->obsVkCaptureEnabled() ? "1" : "0");
    env.insert("PROTON_USE_XALIA", settings->xaliaEnabled() ? "1" : "0");
    env.insert("PROTON_ENABLE_WAYLAND", settings->waylandEnabled() ? "1" : "0");
    env.insert("PROTON_USE_WOW64", settings->wow64Enabled() ? "1" : "0");
    env.insert("PROTON_ENABLE_HDR", settings->hdrEnabled() ? "1" : "0");
    env.insert("PROTON_USE_WINED3D", settings->openglEnabled() ? "1" : "0");
    env.insert("PROTON_ENABLE_NVAPI", settings->nvapiEnabled() ? "1" : "0");
    env.insert("PROTON_USE_SDL", settings->sdlInputEnabled() ? "1" : "0");

    if (useSteam) {
        // Launch using Steam
        m_process.setProgram(ct->path() % "/proton");
        m_process.setArguments({ "run", exeFile.path() });

        // Set current language
        QString protnLocaleName = TRANSLATOR->currentLocale().name() % ".UTF-8";
        env.insert("HOST_LC_ALL", protnLocaleName);
        env.insert("LANG", protnLocaleName);

        // Set Steam system path
        const QDir& steamDir = APP_SETTINGS->steamDir();
        env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH", steamDir.absolutePath());
        env.insert("STEAM_COMPAT_DATA_PATH", prefix->path());

        // Enable Steam Overlay
        const QString& steamOverlay32bit = steamDir.filePath("ubuntu12_32/gameoverlayrenderer.so");
        const QString& steamOverlay64bit = steamDir.filePath("ubuntu12_64/gameoverlayrenderer.so");
        env.insert("LD_PRELOAD", steamOverlay32bit % ":" % steamOverlay64bit);

        if (settings->onlineFixEnabled()) {
            // Redefining DLLs for OnlineFix
            env.insert("WINEDLLOVERRIDES", "steam_api64=n;onlinefix64=n;winpixeventruntime=n,b");
        }
    } else {
        // Launching without of Steam
        m_process.setProgram("umu-run");
        m_process.setArguments({ exeFile.path() });

        env.insert("PROTONPATH", ct->path());
        env.insert("UMU_RUNTIME_UPDATE", APP_SETTINGS->runtimeAutoUpdate() ? "1" : "0");
        env.insert("UMU_USE_STEAM", settings->steamEnvEnabled() ? "1" : "0");
    }

    m_process.setProcessEnvironment(env);
    m_process.setWorkingDirectory(exeFile.dirPath());

    qDebug() << "Use Steam:" << useSteam;
    qDebug() << "Executable:" << exeFile.name();
    qDebug() << "Prefix:" << prefix->name();
    qDebug() << "Compatibility tool:" << ct->name();
    qDebug() << "Runtime auto-update:" << APP_SETTINGS->runtimeAutoUpdate();

    m_process.start();
}

void ProcessManager::runWineCfg(const Prefix* prefix)
{
    runWinetricksUtility("winecfg", prefix);
}

void ProcessManager::runExplorer(const Prefix* prefix)
{
    runWinetricksUtility("explorer", prefix);
}

void ProcessManager::runRegedit(const Prefix* prefix)
{
    runWinetricksUtility("regedit", prefix);
}

void ProcessManager::runUninstaller(const Prefix* prefix)
{
    runWinetricksUtility("uninstaller", prefix);
}

void ProcessManager::runWinetricksUtility(const QString& utilName, const Prefix* prefix)
{
    if (!isValidPrefix(prefix)) {
        showError("Cannot run with empty prefix", RunningError::InvalidPrefix);
        return;
    }

    if (APP_SETTINGS->winetricksPath().isEmpty()) {
        showError("\"winetricks\" not found", RunningError::NoWinetricks);
        return;
    }

    if (QStandardPaths::findExecutable("umu-run").isEmpty()) {
        showError("\"umu-run\" not found", RunningError::NoUmu);
        return;
    }

    static QStringList winetricksUtils { "winecfg", "explorer", "regedit", "uninstaller" };

    if (!winetricksUtils.contains(utilName)) {
        qCritical() << "Unknown winetricks utility";
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("WINEPREFIX", prefix->path());

    m_process.setProcessEnvironment(env);
    m_process.setProgram("umu-run");
    m_process.setArguments({ "winetricks", utilName });

    m_process.start();
}

bool ProcessManager::isReadyToRunExecutable(const ExecutableFile& exeFile)
{
    if (!exeFile.isValid()) {
        showError("The executable file is not valid", RunningError::InvalidExecutable);
        return false;
    }

    if (m_process.state() == QProcess::Running) {
        showError("The executable file is currently running", RunningError::AlreadyRunning);
        return false;
    }

    Prefix* prefix = exeFile.prefix();
    if (!isValidPrefix(prefix)) {
        showError("Cannot run with empty prefix", RunningError::InvalidPrefix);
        return false;
    }

    Ct* ct = prefix->ct();
    if (ct == nullptr || ct->path().isEmpty()) {
        showError("Cannot run with empty compatibility tool", RunningError::InvalidCt);
        return false;
    }

    return true;
}

bool ProcessManager::isValidPrefix(const Prefix* prefix)
{
    return prefix != nullptr && !prefix->path().isEmpty();
}

void ProcessManager::stop()
{
    if (m_process.state() == QProcess::NotRunning) {
        return;
    }

    qDebug() << "Manual termination of the process";
    m_process.terminate();
    if (!m_process.waitForFinished()) {
        qWarning() << "Killing the process after a long wait";
        m_process.kill();
        m_process.waitForFinished();
    }
}

bool ProcessManager::isRunning() const
{
    return m_isRunning;
}

void ProcessManager::onProcessStarted()
{
    m_isRunning = true;
    emit runningChanged(true);
}

void ProcessManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "The process terminated with the code:" << exitCode;
    m_isRunning = false;
    emit runningChanged(false);
}

void ProcessManager::onProcessError(QProcess::ProcessError error)
{
    QString errorText = m_process.errorString();
    switch (error) {
    case QProcess::FailedToStart:
        showError(errorText, RunningError::FailedToStart);
        break;
    case QProcess::Crashed:
        showError(errorText, RunningError::Crashed);
        break;
    case QProcess::Timedout:
        showError(errorText, RunningError::Timedout);
        break;
    case QProcess::ReadError:
        showError(errorText, RunningError::ReadError);
        break;
    case QProcess::WriteError:
        showError(errorText, RunningError::WriteError);
        break;
    default:
        showError(errorText, RunningError::UnknownError);
        break;
    }
}

void ProcessManager::showError(const QString& errorText, RunningError error, bool emitText)
{
    qCritical() << errorText;
    emit runningError(error, emitText ? errorText : "");
}