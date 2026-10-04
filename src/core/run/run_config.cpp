#include "run_config.hpp"

#include "core/appsettings/app_settings.hpp"
#include "core/executablefile/executable_file.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

RunConfig::RunConfig(QObject* parent)
    : QObject(parent)
    , m_exeFile(new ExecutableFile("", this))
{
}

void RunConfig::setExecutablePath(const QString& exePath)
{
    m_exeFile->setPath(exePath);
}

ExecutableFile* RunConfig::exeFile() const
{
    return m_exeFile;
}

QString RunConfig::exePath() const
{
    return m_exeFile->path();
}

QString RunConfig::exeName() const
{
    return m_exeFile->name();
}

const QIcon& RunConfig::exeIcon() const
{
    return m_exeFile->icon();
}

bool RunConfig::exeIsValid() const {
    return m_exeFile->isValid();
}

void RunConfig::setPrefix(Prefix* prefix)
{
    m_prefix = prefix;
}

Prefix* RunConfig::prefix() const
{
    return m_prefix;
}

bool RunConfig::prefixIsValid() const {
    return !m_prefix.isNull();
}

void RunConfig::setCt(Ct* ct)
{
    m_ct = ct;
}

Ct* RunConfig::ct() const
{
    return m_ct;
}

QString RunConfig::workingDirPath() const {
    return m_exeFile->dirPath();
}

QProcessEnvironment& RunConfig::env()
{
    return m_env;
}

QProcessEnvironment& RunConfig::setNewEnv()
{
    return m_env = QProcessEnvironment::systemEnvironment();
}

bool RunConfig::isUsingSteam() const
{
    if (m_prefix) {
        return APP_SETTINGS->steamExists() && m_prefix->settings()->steamEnabled();
    }
    return false;
}