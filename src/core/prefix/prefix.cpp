#include "prefix.hpp"

#include <QCryptographicHash>
#include <QRegularExpression>

#include "core/appsettings/app_settings.hpp"
#include "prefix_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

Prefix::Prefix(const QString& name, QObject* parent)
    : QObject(parent)
    , m_dir(name.isEmpty() ? "" : PREFIXES_DIR.filePath(name))
    , m_name(name)
{
    m_settings = new PrefixSettings(m_dir.filePath(".kisel/prefix.conf"_L1), this);
}

QString Prefix::path() const
{
    return m_dir.absolutePath();
}

QDir Prefix::dir() const
{
    return m_dir;
}

QString Prefix::name() const
{
    return m_name;
}

PrefixSettings* Prefix::settings() const
{
    return m_settings;
}

bool Prefix::exists() const
{
    return m_dir.exists();
}

bool Prefix::makePath()
{
    if (m_name.isEmpty() || !m_dir.mkpath(".")) {
        return false;
    }

    QFile file(m_dir.filePath("pfx.lock"_L1));
    if (file.open(QIODevice::WriteOnly)) {
        file.close();
        return true;
    }

    qDebug() << "Failed to create pfx.lock";
    return false;
}
