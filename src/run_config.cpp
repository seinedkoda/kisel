#include "run_config.hpp"

#include "app_settings.hpp"
#include "executable_file.hpp"
#include "prefix_model.hpp"

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

void RunConfig::setPrefix(Prefix* prefix)
{
    m_prefix = prefix;
}

void RunConfig::setPrefixName(const QString& prefixName)
{
    Prefix* prefix = PREFIX_MODEL->forName(prefixName);
    if (prefix == nullptr) {
        qWarning() << "Prefix" << prefixName << "not found, default prefix used";
        m_prefix = PREFIX_MODEL->defaultPrefix();
    } else {
        m_prefix = prefix;
    }
}

Prefix* RunConfig::prefix() const
{
    return m_prefix;
}

void RunConfig::setCt(Ct* ct)
{
    m_ct = ct;
}

Ct* RunConfig::ct() const
{
    return m_ct;
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