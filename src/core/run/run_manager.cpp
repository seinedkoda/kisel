#include "run_manager.hpp"

#include "core/appsettings/app_settings.hpp"

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
{
}

void RunManager::runExe(RunConfig* config)
{
    if (m_isRunning) {
        showError("The process is currently running", AlreadyRunning);
        return;
    }

    if (!config->exeIsValid()) {
        showError("The executable file is not valid", InvalidExecutable);
        return;
    }

    if (!setupPrefix(config) || !setupCt(config)) {
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList runnerCommandParts;

    if (config->prefixSettings()->steamEnabled()) {
        runnerCommandParts = setupProtonCommand(config, env);
    } else {
        runnerCommandParts = setupUmuCommand(config, env);
    }

    if (!setupProcess(config, runnerCommandParts, env)) {
        return;
    }

    if (APP_SETTINGS->loggingEnabled()) {
        setupExeProcessLogging(config);
    }

    connect(m_process, &QProcess::started, this, &RunManager::onExeProcessStarted);
    connect(m_process, &QProcess::finished, this, &RunManager::onExeProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &RunManager::onExeProcessError);

    m_currentTaskName = config->exeName();
    m_process->start();
}

bool RunManager::setupProcess(RunConfig* config, const QStringList& runnerCommandParts, QProcessEnvironment& env, const QStringList& args)
{
    if (runnerCommandParts.isEmpty()) {
        return false;
    }

    const PrefixSettings* prefixSettings = config->prefixSettings();

    env.insert("WINEPREFIX"_L1, config->prefix()->path());
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

    baseCommandParts << runnerCommandParts << args;
    m_fullCommandString = baseCommandParts.join(" ");
    QString program = baseCommandParts.takeFirst();

    m_process = new QProcess(this);
    m_process->setWorkingDirectory(config->workingDirPath());
    m_process->setProcessEnvironment(env);
    m_process->setProgram(program);
    m_process->setArguments(baseCommandParts);

    return true;
}

bool RunManager::setupPrefix(RunConfig* config)
{
    Prefix* prefix = config->prefix();

    if (prefix == nullptr || prefix->name().isEmpty()) {
        qWarning() << "Prefix not found, default prefix used";
        prefix = m_prefixModel->defaultPrefix();
        config->setPrefix(prefix);
    }

    if (!prefix->exists() && !prefix->makePath()) {
        showError("Failed to write prefix", PrefixWriteError);
        return false;
    }

    m_prefixModel->refreshList();
    return true;
}

bool RunManager::setupCt(RunConfig* config)
{
    Ct* ct = config->ct();
    PrefixSettings* prefixSettings = config->prefixSettings();
    const QString prefixCtPath = prefixSettings->ctPath();

    if (ct == nullptr || ct->path().isEmpty()) {
        Ct* prefixCt = m_ctModel->getByPath(prefixCtPath);
        if (prefixCt != nullptr) {
            config->setCt(prefixCt);
        } else {
            Ct* defaultCt = m_ctModel->defaultCt();
            if (defaultCt == nullptr || defaultCt->path().isEmpty()) {
                showError("Cannot run with empty compatibility tool", InvalidCt);
                return false;
            }
            config->setCt(defaultCt);
        }
    }

    QString runConfigCtPath = config->ct()->path();
    if (prefixCtPath != runConfigCtPath) {
        prefixSettings->setCtPath(runConfigCtPath); // Save run settings
    }
    return true;
}

QStringList RunManager::setupProtonCommand(RunConfig* config, QProcessEnvironment& env)
{
    if (config->exePath().isEmpty()) {
        return { };
    }

    const Prefix* prefix = config->prefix();

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
        QFileInfo exeFileInfo(config->exePath());
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

    return { config->ct()->dir().filePath("/proton"_L1), "run"_L1, config->exePath() };
}

QStringList RunManager::setupUmuCommand(RunConfig* config, QProcessEnvironment& env)
{
    QString umuPath = APP_SETTINGS->umuPath();
    if (umuPath.isEmpty()) {
        showError("Runner not found", NoUmu);
        return { };
    }

    QStringList commandParts { umuPath };

    const ExecutableFile* exeFile = config->exeFile();
    const PrefixSettings* prefixSettings = config->prefixSettings();
    const QString& exePath = exeFile->path();

    if (exeFile->isMsi()) {
        commandParts.append({ "msiexec"_L1, "/i"_L1, exePath });
    } else if (exeFile->isCmd()) {
        commandParts.append({ "cmd"_L1, "/c"_L1, exePath });
    } else if (!exePath.isEmpty()) {
        commandParts.append(exePath);
    }

    env.insert("PROTONPATH"_L1, config->ct()->path());
    env.insert("GAMEID"_L1, prefixSettings->gameId());
    env.insert("STORE"_L1, prefixSettings->store());
    env.insert("UMU_RUNTIME_UPDATE"_L1, APP_SETTINGS->runtimeAutoUpdate() ? Y : N);
    env.insert("UMU_USE_STEAM"_L1, prefixSettings->steamEnvEnabled() ? Y : N);
    env.insert("UMU_LOG"_L1, APP_SETTINGS->loggingEnabled() ? Y : N);

    return commandParts;
}

void RunManager::setupExeProcessLogging(RunConfig* config)
{
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setStandardOutputFile(APP_SETTINGS->logFilePath(), QIODevice::Append);

    qDebug() << "=== START EXECUTABLE PROCESS LOGGING" << QDateTime::currentDateTime().toString(Qt::ISODate) << "===";
    qDebug() << "EXECUTABLE:" << config->exePath();
    qDebug() << "PREFIX:" << config->prefix()->path();
    qDebug() << "COMPATIBILITY TOOL:" << config->ct()->path();
    qDebug() << "USE STEAM:" << static_cast<int>(config->isUsingSteam());
    if (!config->isUsingSteam()) {
        qDebug() << "UMU:" << APP_SETTINGS->umuPath();
        qDebug() << "RUNTIME AUTO-UPDATE:" << APP_SETTINGS->runtimeAutoUpdate();
    }
    qDebug() << "COMMAND:" << m_fullCommandString;
    qDebug() << "===================\n";
}

void RunManager::runWineCfg(Prefix* prefix)
{
    runWinetricks(prefix, { "winecfg"_L1 }, "Winetricks - winecfg"_L1);
}

void RunManager::runExplorer(Prefix* prefix)
{
    runWinetricks(prefix, { "explorer"_L1 }, "Winetricks - explorer"_L1);
}

void RunManager::runRegedit(Prefix* prefix)
{
    runWinetricks(prefix, { "regedit"_L1 }, "Winetricks - regedit"_L1);
}

void RunManager::runUninstaller(Prefix* prefix)
{
    runWinetricks(prefix, { "uninstaller"_L1 }, "Winetricks - uninstaller"_L1);
}

void RunManager::runComponentsList(Prefix* prefix, const QString& category, WinetricksCallback callback)
{
    runWinetricks(prefix, { category, "list"_L1 }, tr("Updating the component list"), callback);
}

void RunManager::runInstalledComponentsList(Prefix* prefix, const QString& category, WinetricksCallback callback)
{
    runWinetricks(prefix, { category, "list-installed"_L1 }, tr("Updating the list of installed components"), callback);
}

void RunManager::runComponentsInstallation(Prefix* prefix, const QStringList& components, WinetricksCallback callback)
{
    runWinetricks(prefix, QStringList() << "-q"_L1 << components, tr("Installing components"), callback);
}

void RunManager::runWinetricks(Prefix* prefix, const QStringList& args, const QString& taskName, WinetricksCallback callback)
{
    if (m_isRunning) {
        showError("The process is currently running", AlreadyRunning);
        return;
    }

    if (APP_SETTINGS->winetricksPath().isEmpty()) {
        showError("\"winetricks\" not found", NoWinetricks);
        return;
    }

    RunConfig config;
    config.setPrefix(prefix);

    if (!setupPrefix(&config) || !setupCt(&config)) {
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList umuCommandParts = setupUmuCommand(&config, env);
    QStringList winetricksCommandParts = QStringList() << "winetricks"_L1 << args;

    // Don't use pure winetricks!
    if (!setupProcess(&config, umuCommandParts, env, winetricksCommandParts)) {
        return;
    }

    connect(m_process, &QProcess::started, this, [this] {
        m_isRunning = true;
        emit runningChanged(true);
    });

    QObject::connect(m_process, &QProcess::finished, this,
        [this, callback](int exitCode, QProcess::ExitStatus exitStatus) {
            qInfo() << "The process terminated with the code:" << exitCode;

            QString output = QString::fromUtf8(m_process->readAllStandardOutput());

            if (!output.isEmpty()) {
                qDebug().noquote().nospace() << "=== WINETRICKS OUTPUT ===\n"
                                             << output << "\n=== END OF OUTPUT ===";
            }

            m_currentTaskName.clear();
            m_isRunning = false;
            emit runningChanged(false);
            m_process->deleteLater();
            m_process = nullptr;

            if (callback) {
                callback(exitCode, exitStatus, output);
            }
        });

    qDebug() << "=== START WINETRICKS ===\nCOMMAND:" << m_fullCommandString << "\nPREFIX:" << prefix->path() << "\n======";
    m_currentTaskName = taskName.isEmpty() ? "Winetricks"_L1 : taskName;
    m_process->start();
}

void RunManager::stop()
{
    if (!m_isRunning) {
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

void RunManager::onExeProcessStarted()
{
    m_isRunning = true;
    emit runningChanged(true, true);
}

void RunManager::onExeProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_currentTaskName.clear();
    qInfo() << "The process terminated with the code:" << exitCode;
    m_isRunning = false;
    emit runningChanged(false, true);
    m_process->deleteLater();
    m_process = nullptr;
}

void RunManager::onExeProcessError(QProcess::ProcessError error)
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
    qCritical().noquote() << errorText;
    emit exeRunningError(error, emitText ? errorText : "");
}

QString RunManager::taskName() const
{
    return m_currentTaskName;
}
