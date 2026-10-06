#include "run_manager.hpp"

#include "core/appsettings/app_settings.hpp"
#include "core/executablefile/executable_file.hpp"
#include "run_config.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

static const auto Y = "1"_L1;
static const auto N = "0"_L1;

RunManager::RunManager(PrefixModel* prefixModel, CtModel* ctModel, QObject* parent)
    : QObject(parent)
    , m_prefixModel(prefixModel)
    , m_ctModel(ctModel)
    , m_process(nullptr)
    , m_isRunning(false)
    , m_runConfig(new RunConfig(this))
{
}

RunConfig* RunManager::config()
{
    return m_runConfig;
}

void RunManager::run()
{
    if (m_isRunning) {
        showError("The executable file is currently running", AlreadyRunning);
        return;
    }

    if (!setupConfigData()) {
        return;
    }

    if (!setupProcess()) {
        return;
    }

    if (APP_SETTINGS->loggingEnabled()) {
        setupExeProcessLogging();
    }

    m_currentTaskName = m_runConfig->exeName();
    m_process->start();
}

bool RunManager::setupConfigData()
{
    if (!m_runConfig->exeIsValid()) {
        showError("The executable file is not valid", InvalidExecutable);
        return false;
    }

    if (!setupPrefix()) {
        return false;
    }

    if (!setupCt()) {
        return false;
    }

    return true;
}

bool RunManager::setupProcess()
{
    const PrefixSettings* prefixSettings = m_runConfig->prefix()->settings();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("WINEPREFIX"_L1, m_runConfig->prefix()->path());
    env.insert("MANGOHUD"_L1, prefixSettings->mangoHudEnabled() ? Y : N);
    env.insert("OBS_VKCAPTURE"_L1, prefixSettings->obsVkCaptureEnabled() ? Y : N);
    env.insert("PROTON_USE_XALIA"_L1, prefixSettings->xaliaEnabled() ? Y : N);
    env.insert("PROTON_ENABLE_WAYLAND"_L1, prefixSettings->waylandEnabled() ? Y : N);
    env.insert("PROTON_USE_WOW64"_L1, prefixSettings->wow64Enabled() ? Y : N);
    env.insert("PROTON_ENABLE_HDR"_L1, prefixSettings->hdrEnabled() ? Y : N);
    env.insert("PROTON_USE_WINED3D"_L1, prefixSettings->openglEnabled() ? Y : N);
    env.insert("PROTON_ENABLE_NVAPI"_L1, prefixSettings->nvapiEnabled() ? Y : N);
    env.insert("PROTON_USE_SDL"_L1, prefixSettings->sdlInputEnabled() ? Y : N);

    QStringList baseCommandParts;

    const QString& gamescopePath = APP_SETTINGS->gamescopePath();
    if (prefixSettings->gamescopeEnabled() && !gamescopePath.isEmpty()) {
        baseCommandParts << gamescopePath << QProcess::splitCommand(prefixSettings->gamescopeArgs()) << "--"_L1;
    }

    QStringList runnerCommandParts;
    if (m_runConfig->isUsingSteam()) {
        runnerCommandParts << setupProtonCommand(env);
    } else {
        runnerCommandParts << setupUmuCommand(env);
    }

    if (runnerCommandParts.isEmpty()) {
        return false;
    }

    baseCommandParts << runnerCommandParts;
    QString program = baseCommandParts.takeFirst();

    m_process = new QProcess(this);
    m_process->setWorkingDirectory(m_runConfig->workingDirPath());
    m_process->setProcessEnvironment(env);
    m_process->setProgram(program);
    m_process->setArguments(baseCommandParts);

    connect(m_process, &QProcess::started, this, &RunManager::onProcessStarted);
    connect(m_process, &QProcess::finished, this, &RunManager::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &RunManager::onProcessError);

    return true;
}

bool RunManager::setupPrefix()
{
    Prefix* prefix = m_runConfig->prefix();

    if (prefix == nullptr || prefix->name().isEmpty()) {
        qWarning() << "Prefix not found, default prefix used";
        prefix = m_prefixModel->defaultPrefix();
        m_runConfig->setPrefix(prefix);
    }

    if (!prefix->exists() && !prefix->makePath()) {
        showError("Failed to write prefix", PrefixWriteError);
        return false;
    }

    m_prefixModel->refreshList();
    return true;
}

bool RunManager::setupCt()
{
    Ct* ct = m_runConfig->ct();
    PrefixSettings* prefixSettings = m_runConfig->prefix()->settings();
    const QString prefixCtPath = prefixSettings->ctPath();

    if (ct == nullptr || ct->path().isEmpty()) {
        Ct* prefixCt = m_ctModel->getByPath(prefixCtPath);
        if (prefixCt != nullptr) {
            m_runConfig->setCt(prefixCt);
        } else {
            Ct* defaultCt = m_ctModel->defaultCt();
            if (defaultCt == nullptr || defaultCt->path().isEmpty()) {
                showError("Cannot run with empty compatibility tool", InvalidCt);
                return false;
            }
            m_runConfig->setCt(defaultCt);
        }
    }

    QString runConfigCtPath = m_runConfig->ct()->path();
    if (prefixCtPath != runConfigCtPath) {
        prefixSettings->setCtPath(runConfigCtPath); // Save run settings
    }
    return true;
}

QStringList RunManager::setupProtonCommand(QProcessEnvironment& env)
{
    const Prefix* prefix = m_runConfig->prefix();

    // Set current language
    QString protonLocaleName = APP_SETTINGS->locale() % ".UTF-8"_L1;
    env.insert("HOST_LC_ALL"_L1, protonLocaleName);
    env.insert("LANG"_L1, protonLocaleName);

    // Set Steam system path
    const QDir& steamDir = APP_SETTINGS->steamDir();
    env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH"_L1, steamDir.absolutePath());
    env.insert("STEAM_COMPAT_DATA_PATH"_L1, prefix->path());

    if (prefix->settings()->steamOverlayEnabled()) {
        const QString& steamOverlay32bit = steamDir.filePath("ubuntu12_32/gameoverlayrenderer.so"_L1);
        const QString& steamOverlay64bit = steamDir.filePath("ubuntu12_64/gameoverlayrenderer.so"_L1);
        env.insert("LD_PRELOAD"_L1, steamOverlay32bit % ":"_L1 % steamOverlay64bit);

        // The LD_PRELOAD does nothing if the vulkan layer is not enabled with this
        env.insert("ENABLE_VK_LAYER_VALVE_steam_overlay_1"_L1, "1"_L1);

        // Steam overlay requires a non empty value on SteamGameId
        // If steam_appid.txt exists use it otherwise 480 (Spacewar)
        QFileInfo exeFileInfo(m_runConfig->exePath());
        QFile steamAppId(exeFileInfo.dir().filePath("steam_appid.txt"_L1));
        if (steamAppId.open(QIODevice::ReadOnly)) {
            env.insert("SteamGameId"_L1, steamAppId.readAll().trimmed());
        } else {
            env.insert("SteamGameId"_L1, "480"_L1);
        }
    }

    if (prefix->settings()->onlineFixEnabled()) {
        // Redefining DLLs for OnlineFix
        env.insert("WINEDLLOVERRIDES"_L1, "steam_api64=n;onlinefix64=n;winpixeventruntime=n,b"_L1);
    }

    return { m_runConfig->ct()->dir().filePath("/proton"_L1), "run"_L1, m_runConfig->exePath() };
}

QStringList RunManager::setupUmuCommand(QProcessEnvironment& env)
{
    QString umuPath = APP_SETTINGS->umuPath();
    if (umuPath.isEmpty()) {
        showError("Runner not found", NoUmu);
        return { };
    }

    QStringList commandParts { umuPath };

    const ExecutableFile* exeFile = m_runConfig->exeFile();
    const PrefixSettings* prefixSettings = m_runConfig->prefix()->settings();

    if (exeFile->isMsi()) {
        commandParts.append({ "msiexec", "/i", exeFile->path() });
    } else if (exeFile->isCmd()) {
        commandParts.append({ "cmd", "/c", exeFile->path() });
    } else {
        commandParts.append(exeFile->path());
    }

    env.insert("PROTONPATH"_L1, m_runConfig->ct()->path());
    env.insert("GAMEID"_L1, prefixSettings->gameId());
    env.insert("STORE"_L1, prefixSettings->store());
    env.insert("UMU_RUNTIME_UPDATE"_L1, APP_SETTINGS->runtimeAutoUpdate() ? Y : N);
    env.insert("UMU_USE_STEAM"_L1, prefixSettings->steamEnvEnabled() ? Y : N);
    env.insert("UMU_LOG"_L1, APP_SETTINGS->loggingEnabled() ? Y : N);

    return commandParts;
}

void RunManager::setupExeProcessLogging()
{
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setStandardOutputFile(APP_SETTINGS->logFilePath(), QIODevice::Append);

    qDebug() << "=== START EXECUTABLE PROCESS LOGGING" << QDateTime::currentDateTime().toString(Qt::ISODate) << "===";
    qDebug() << "EXECUTABLE:" << m_runConfig->exePath();
    qDebug() << "USE STEAM:" << static_cast<int>(m_runConfig->isUsingSteam());
    if (!m_runConfig->isUsingSteam()) {
        qDebug() << "UMU:" << APP_SETTINGS->umuPath();
    }
    qDebug() << "PREFIX:" << m_runConfig->prefix()->path();
    qDebug() << "COMPATIBILITY TOOL:" << m_runConfig->ct()->path();
    qDebug() << "RUNTIME AUTO-UPDATE:" << APP_SETTINGS->runtimeAutoUpdate();
    qDebug() << "===================\n";
}

void RunManager::runWineCfg(const Prefix* prefix)
{
    runWinetricksUtility(prefix, "winecfg"_L1);
}

void RunManager::runExplorer(const Prefix* prefix)
{
    runWinetricksUtility(prefix, "explorer"_L1);
}

void RunManager::runRegedit(const Prefix* prefix)
{
    runWinetricksUtility(prefix, "regedit"_L1);
}

void RunManager::runUninstaller(const Prefix* prefix)
{
    runWinetricksUtility(prefix, "uninstaller"_L1);
}

void RunManager::runWinetricksUtility(const Prefix* prefix, const QString& utilName)
{
    if (APP_SETTINGS->winetricksPath().isEmpty()) {
        showError("\"winetricks\" not found", NoWinetricks);
        return;
    }

    if (APP_SETTINGS->umuPath().isEmpty()) {
        showError("\"umu-run\" not found", NoUmu);
        return;
    }

    static QStringList winetricksUtils { "winecfg"_L1, "explorer"_L1, "regedit"_L1, "uninstaller"_L1 };

    if (!winetricksUtils.contains(utilName)) {
        qCritical() << "Unknown winetricks utility";
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("UMU_RUNTIME_UPDATE"_L1, APP_SETTINGS->runtimeAutoUpdate() ? Y : N);
    env.insert("WINEPREFIX"_L1, prefix->path());
    env.insert("PROTONPATH"_L1, prefix->settings()->ctPath());

    m_process->setProcessEnvironment(env);
    m_process->setProgram(APP_SETTINGS->umuPath()); // Don't use pure winetricks!
    m_process->setArguments({ "winetricks", utilName });

    m_currentTaskName = utilName;

    if (APP_SETTINGS->loggingEnabled()) {
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        m_process->setStandardOutputFile(APP_SETTINGS->logFilePath(), QIODevice::Append);
        qDebug() << "START WINETRICKS UTILITY:" << m_currentTaskName;
    }

    m_process->start();
}

void RunManager::stop()
{
    if (m_process->state() == QProcess::NotRunning) {
        return;
    }

    qInfo() << "Manual termination of the process";
    m_process->terminate();
    if (!m_process->waitForFinished()) {
        qWarning() << "Killing the process after a long wait";
        m_process->kill();
        m_process->waitForFinished();
    }
}

bool RunManager::isRunning() const
{
    return m_isRunning;
}

void RunManager::onProcessStarted()
{
    m_isRunning = true;
    emit runningChanged(true);
}

void RunManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_currentTaskName.clear();
    qInfo() << "The process terminated with the code:" << exitCode;
    m_isRunning = false;
    emit runningChanged(false);
    m_process->deleteLater();
    m_process = nullptr;
}

void RunManager::onProcessError(QProcess::ProcessError error)
{
    QString errorText = m_process->errorString();
    switch (error) {
    case QProcess::FailedToStart:
        showError(errorText, FailedToStart, true);
        break;
    case QProcess::Crashed:
        showError(errorText, Crashed, true);
        break;
    case QProcess::Timedout:
        showError(errorText, Timedout, true);
        break;
    case QProcess::ReadError:
        showError(errorText, ReadError, true);
        break;
    case QProcess::WriteError:
        showError(errorText, WriteError, true);
        break;
    default:
        showError(errorText, UnknownError, true);
        break;
    }
}

void RunManager::showError(const QString& errorText, RunningError error, bool emitText)
{
    qCritical() << errorText;
    emit runningError(error, emitText ? errorText : "");
}

QString RunManager::taskName() const
{
    return m_currentTaskName;
}