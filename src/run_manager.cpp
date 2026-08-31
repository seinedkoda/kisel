#include "run_manager.hpp"

#include "app_settings.hpp"
#include "ct_model.hpp"
#include "executable_file.hpp"
#include "prefix_model.hpp"
#include "run_config.hpp"
#include "translator.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

static const auto Y = "1"_L1;
static const auto N = "0"_L1;

RunManager::RunManager(QObject* parent)
    : QObject(parent)
    , m_runConfig(nullptr)
{
    connect(&m_process, &QProcess::started, this, &RunManager::onProcessStarted);
    connect(&m_process, &QProcess::finished, this, &RunManager::onProcessFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &RunManager::onProcessError);
}

RunManager* RunManager::instance()
{
    static RunManager instance;
    return &instance;
}

void RunManager::run(RunConfig* runConfig)
{
    if (m_process.state() == QProcess::Running) {
        showError("The executable file is currently running", AlreadyRunning);
        return;
    }

    if (!setupConfig(runConfig)) {
        return;
    }

    if (runConfig->isUsingSteam()) {
        setupProtonProcess();
    } else {
        setupUmuProcess();
    }

    if (APP_SETTINGS->loggingEnabled()) {
        setupLogging();
    }

    m_process.setProcessEnvironment(runConfig->env());
    m_process.setWorkingDirectory(runConfig->exeFile()->dirPath());
    m_currentTaskName = runConfig->exeName();
    m_process.start();
}

bool RunManager::setupConfig(RunConfig* runConfig)
{
    m_runConfig = runConfig;

    if (!runConfig->exeFile()->isValid()) {
        showError("The executable file is not valid", InvalidExecutable);
        return false;
    }

    if (APP_SETTINGS->umuPath().isEmpty()) {
        showError("\"umu-run\" not found", NoUmu);
        return false;
    }

    if (!setupPrefix()) {
        return false;
    }

    if (!setupCt()) {
        return false;
    }

    QProcessEnvironment& env = runConfig->setNewEnv();
    const PrefixSettings* prefixSettings = runConfig->prefix()->settings();

    env.insert("WINEPREFIX"_L1, runConfig->prefix()->path());
    env.insert("MANGOHUD"_L1, prefixSettings->mangoHudEnabled() ? Y : N);
    env.insert("OBS_VKCAPTURE"_L1, prefixSettings->obsVkCaptureEnabled() ? Y : N);
    env.insert("PROTON_USE_XALIA"_L1, prefixSettings->xaliaEnabled() ? Y : N);
    env.insert("PROTON_ENABLE_WAYLAND"_L1, prefixSettings->waylandEnabled() ? Y : N);
    env.insert("PROTON_USE_WOW64"_L1, prefixSettings->wow64Enabled() ? Y : N);
    env.insert("PROTON_ENABLE_HDR"_L1, prefixSettings->hdrEnabled() ? Y : N);
    env.insert("PROTON_USE_WINED3D"_L1, prefixSettings->openglEnabled() ? Y : N);
    env.insert("PROTON_ENABLE_NVAPI"_L1, prefixSettings->nvapiEnabled() ? Y : N);
    env.insert("PROTON_USE_SDL"_L1, prefixSettings->sdlInputEnabled() ? Y : N);

    return true;
}

bool RunManager::setupPrefix() {
    Prefix* prefix = m_runConfig->prefix();

    if (prefix == nullptr || prefix->name().isEmpty()) {
        prefix = PREFIX_MODEL->defaultPrefix();
        m_runConfig->setPrefix(prefix);
    }

    if (!prefix->exists()) {
        if (!prefix->makePath()) {
            showError("Failed to write prefix", PrefixWriteError);
            return false;
        }
    }

    PREFIX_MODEL->refreshList();
    return true;
}

bool RunManager::setupCt() {
    Ct* ct = m_runConfig->ct();
    PrefixSettings* prefixSettings = m_runConfig->prefix()->settings();
    const QString prefixCtPath = prefixSettings->ctPath();

    if (ct == nullptr || ct->path().isEmpty()) {
        Ct* prefixCt = CT_MODEL->forPath(prefixCtPath);
        if (prefixCt != nullptr) {
            m_runConfig->setCt(prefixCt);
        } else {
            Ct* defaultCt = CT_MODEL->defaultCt();
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

void RunManager::setupProtonProcess()
{
    const Prefix* prefix = m_runConfig->prefix();
    QProcessEnvironment& env = m_runConfig->env();

    m_process.setProgram(m_runConfig->ct()->path() % "/proton"_L1);
    m_process.setArguments({ "run"_L1, m_runConfig->exePath() });

    // Set current language
    QString protnLocaleName = TRANSLATOR->currentLocale().name() % ".UTF-8"_L1;
    env.insert("HOST_LC_ALL"_L1, protnLocaleName);
    env.insert("LANG"_L1, protnLocaleName);

    // Set Steam system path
    const QDir& steamDir = APP_SETTINGS->steamDir();
    env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH"_L1, steamDir.absolutePath());
    env.insert("STEAM_COMPAT_DATA_PATH"_L1, prefix->path());

    if (prefix->settings()->steamOverlayEnabled()) {
        const QString& steamOverlay32bit = steamDir.filePath("ubuntu12_32/gameoverlayrenderer.so"_L1);
        const QString& steamOverlay64bit = steamDir.filePath("ubuntu12_64/gameoverlayrenderer.so"_L1);
        env.insert("LD_PRELOAD"_L1, steamOverlay32bit % ":"_L1 % steamOverlay64bit);
    }

    if (prefix->settings()->onlineFixEnabled()) {
        // Redefining DLLs for OnlineFix
        env.insert("WINEDLLOVERRIDES"_L1, "steam_api64=n;onlinefix64=n;winpixeventruntime=n,b"_L1);
    }
}

void RunManager::setupUmuProcess()
{
    const ExecutableFile* exeFile = m_runConfig->exeFile();
    const PrefixSettings* prefixSettings = m_runConfig->prefix()->settings();
    QProcessEnvironment& env = m_runConfig->env();

    m_process.setProgram(APP_SETTINGS->umuPath());
    QStringList args;
    if (m_runConfig->exeFile()->isMsi()) {
        args.append({ "msiexec", "/i", exeFile->path() });
    } else if (exeFile->isCmd()) {
        args.append({ "cmd", "/c", exeFile->path() });
    } else {
        args.append(exeFile->path());
    }
    m_process.setArguments(args);

    env.insert("PROTONPATH"_L1, m_runConfig->ct()->path());
    env.insert("UMU_RUNTIME_UPDATE"_L1, APP_SETTINGS->runtimeAutoUpdate() ? Y : N);
    env.insert("UMU_USE_STEAM"_L1, prefixSettings->steamEnvEnabled() ? Y : N);
    env.insert("UMU_LOG"_L1, APP_SETTINGS->loggingEnabled() ? Y : N);
}

void RunManager::setupLogging()
{
    QFile logFile(APP_SETTINGS->logFilePath());
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream stream(&logFile);

        stream << "=== kisel "_L1 << APP_VERSION << " ===\n"_L1;
        stream << "OS: "_L1 << QSysInfo::prettyProductName() << u'\n';
        stream << "TIME: " << QDateTime::currentDateTime().toString(Qt::ISODate) << u'\n';
        stream << "USE STEAM: "_L1 << static_cast<int>(m_runConfig->isUsingSteam()) << u'\n';
        if (!m_runConfig->isUsingSteam()) {
            stream << "UMU: "_L1 << APP_SETTINGS->umuPath() << u'\n';
        }
        stream << "EXECUTABLE: "_L1 << m_runConfig->exePath() << u'\n';
        stream << "PREFIX: "_L1 << m_runConfig->prefix()->path() << u'\n';
        stream << "COMPATIBILITY TOOL: "_L1 << m_runConfig->ct()->path() << u'\n';
        stream << "RUNTIME AUTO-UPDATE: " << APP_SETTINGS->runtimeAutoUpdate() << u'\n';
        stream << "===================\n\n"_L1;

        logFile.close();
    }

    m_process.setProcessChannelMode(QProcess::MergedChannels);
    m_process.setStandardOutputFile(APP_SETTINGS->logFilePath(), QIODevice::Append);
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

    m_process.setProcessEnvironment(env);
    m_process.setProgram(APP_SETTINGS->umuPath()); // Don't use pure winetricks!
    m_process.setArguments({ "winetricks", utilName });

    m_currentTaskName = utilName;

    m_process.start();
}

void RunManager::stop()
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
    qDebug() << "The process terminated with the code:" << exitCode;
    m_isRunning = false;
    emit runningChanged(false);
}

void RunManager::onProcessError(QProcess::ProcessError error)
{
    QString errorText = m_process.errorString();
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