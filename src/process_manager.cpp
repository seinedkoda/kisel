#include "process_manager.hpp"

#include <QStandardPaths>

#include "app_settings.hpp"
#include "ct_model.hpp"
#include "executable_file.hpp"
#include "prefix_model.hpp"
#include "translator.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

ProcessManager::ProcessManager(QObject* parent)
    : QObject(parent)
{
    connect(&m_process, &QProcess::started, this, &ProcessManager::onProcessStarted);
    connect(&m_process, &QProcess::finished, this, &ProcessManager::onProcessFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &ProcessManager::onProcessError);
}

ProcessManager* ProcessManager::instance()
{
    static ProcessManager instance;
    return &instance;
}

void ProcessManager::run(const ExecutableFile& exeFile, RunConfig* runConfig)
{
    if (!preRunCheck(exeFile, runConfig)) {
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const Prefix* prefix = runConfig->prefix();
    PrefixSettings* prefixSettings = prefix->settings();
    const Ct* ct = runConfig->ct();
    PREFIX_MODEL->refreshList();

    // Save run settings
    prefixSettings->setCtPath(ct->path());

    bool useSteam = APP_SETTINGS->steamExists() && prefixSettings->steamEnabled();

    static auto y = "1"_L1;
    static auto n = "0"_L1;

    env.insert("WINEPREFIX"_L1, prefix->path());
    env.insert("MANGOHUD"_L1, prefixSettings->mangoHudEnabled() ? y : n);
    env.insert("OBS_VKCAPTURE"_L1, prefixSettings->obsVkCaptureEnabled() ? y : n);
    env.insert("PROTON_USE_XALIA"_L1, prefixSettings->xaliaEnabled() ? y : n);
    env.insert("PROTON_ENABLE_WAYLAND"_L1, prefixSettings->waylandEnabled() ? y : n);
    env.insert("PROTON_USE_WOW64"_L1, prefixSettings->wow64Enabled() ? y : n);
    env.insert("PROTON_ENABLE_HDR"_L1, prefixSettings->hdrEnabled() ? y : n);
    env.insert("PROTON_USE_WINED3D"_L1, prefixSettings->openglEnabled() ? y : n);
    env.insert("PROTON_ENABLE_NVAPI"_L1, prefixSettings->nvapiEnabled() ? y : n);
    env.insert("PROTON_USE_SDL"_L1, prefixSettings->sdlInputEnabled() ? y : n);

    if (useSteam) {
        // Launch using Steam
        m_process.setProgram(ct->path() % "/proton"_L1);
        m_process.setArguments({ "run"_L1, exeFile.path() });

        // Set current language
        QString protnLocaleName = TRANSLATOR->currentLocale().name() % ".UTF-8"_L1;
        env.insert("HOST_LC_ALL"_L1, protnLocaleName);
        env.insert("LANG"_L1, protnLocaleName);

        // Set Steam system path
        const QDir& steamDir = APP_SETTINGS->steamDir();
        env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH"_L1, steamDir.absolutePath());
        env.insert("STEAM_COMPAT_DATA_PATH"_L1, prefix->path());

        // Enable Steam Overlay
        const QString& steamOverlay32bit = steamDir.filePath("ubuntu12_32/gameoverlayrenderer.so"_L1);
        const QString& steamOverlay64bit = steamDir.filePath("ubuntu12_64/gameoverlayrenderer.so"_L1);
        env.insert("LD_PRELOAD"_L1, steamOverlay32bit % ":"_L1 % steamOverlay64bit);

        if (prefixSettings->onlineFixEnabled()) {
            // Redefining DLLs for OnlineFix
            env.insert("WINEDLLOVERRIDES"_L1, "steam_api64=n;onlinefix64=n;winpixeventruntime=n,b"_L1);
        }
    } else {
        // Launching without of Steam
        m_process.setProgram(APP_SETTINGS->umuPath());
        m_process.setArguments({ exeFile.path() });

        env.insert("PROTONPATH"_L1, ct->path());
        env.insert("UMU_RUNTIME_UPDATE"_L1, APP_SETTINGS->runtimeAutoUpdate() ? y : n);
        env.insert("UMU_USE_STEAM"_L1, prefixSettings->steamEnvEnabled() ? y : n);
    }

    m_process.setProcessEnvironment(env);
    m_process.setWorkingDirectory(exeFile.dirPath());

    qDebug() << "Use Steam:"_L1 << useSteam;
    qDebug() << "Executable:"_L1 << exeFile.name();
    qDebug() << "Prefix:"_L1 << prefix->name();
    qDebug() << "Compatibility tool:"_L1 << ct->name();
    qDebug() << "Runtime auto-update:"_L1 << APP_SETTINGS->runtimeAutoUpdate();

    m_process.start();
}

void ProcessManager::runWineCfg(const Prefix* prefix)
{
    runWinetricksUtility("winecfg"_L1, prefix);
}

void ProcessManager::runExplorer(const Prefix* prefix)
{
    runWinetricksUtility("explorer"_L1, prefix);
}

void ProcessManager::runRegedit(const Prefix* prefix)
{
    runWinetricksUtility("regedit"_L1, prefix);
}

void ProcessManager::runUninstaller(const Prefix* prefix)
{
    runWinetricksUtility("uninstaller"_L1, prefix);
}

void ProcessManager::runWinetricksUtility(const QString& utilName, const Prefix* prefix)
{
    if (APP_SETTINGS->winetricksPath().isEmpty()) {
        showError("\"winetricks\" not found", RunningError::NoWinetricks);
        return;
    }

    if (APP_SETTINGS->umuPath().isEmpty()) {
        showError("\"umu-run\" not found", RunningError::NoUmu);
        return;
    }

    static QStringList winetricksUtils { "winecfg"_L1, "explorer"_L1, "regedit"_L1, "uninstaller"_L1 };

    if (!winetricksUtils.contains(utilName)) {
        qCritical() << "Unknown winetricks utility";
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("WINEPREFIX"_L1, prefix->path());

    m_process.setProcessEnvironment(env);
    m_process.setProgram(APP_SETTINGS->umuPath());
    m_process.setArguments({ APP_SETTINGS->winetricksPath(), utilName });

    m_process.start();
}

bool ProcessManager::preRunCheck(const ExecutableFile& exeFile, RunConfig* runConfig)
{
    if (!exeFile.isValid()) {
        showError("The executable file is not valid", RunningError::InvalidExecutable);
        return false;
    }

    if (m_process.state() == QProcess::Running) {
        showError("The executable file is currently running", RunningError::AlreadyRunning);
        return false;
    }

    if (APP_SETTINGS->umuPath().isEmpty()) {
        showError("\"umu-run\" not found", RunningError::NoUmu);
        return false;
    }

    Prefix* prefix = runConfig->prefix();
    if (prefix == nullptr || prefix->name().isEmpty()) {
        prefix = PREFIX_MODEL->defaultPrefix();
        runConfig->setPrefix(prefix);
    }

    if (!prefix->exists()) {
        if (!prefix->makePath()) {
            showError("Failed to write prefix", RunningError::PrefixWriteError);
            return false;
        }
    }

    Ct* ct = runConfig->ct();
    if (ct == nullptr || ct->path().isEmpty()) {
        Ct* defaultCt = CT_MODEL->defaultCt();
        if (defaultCt == nullptr || defaultCt->path().isEmpty()) {
            showError("Cannot run with empty compatibility tool", RunningError::InvalidCt);
            return false;
        }
        runConfig->setCt(defaultCt);
    }

    return true;
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