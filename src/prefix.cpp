#include "prefix.hpp"

#include <QCryptographicHash>
#include <QRegularExpression>

#include "app_settings.hpp"
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

QString Prefix::generatePrefixNameFromFile(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString cleanName = fileInfo.baseName().toLower();

    // Replace any special characters and spaces with a hyphen
    static const QRegularExpression nonAlphaNum("[^a-z0-9-]+"_L1);
    cleanName.replace(nonAlphaNum, "-"_L1);

    // Removing duplicate and hanging hyphens
    static const QRegularExpression multiHyphen("-+"_L1);
    cleanName.replace(multiHyphen, "-"_L1);
    cleanName = cleanName.trimmed();
    static QRegularExpression hangHyphen("^-+|-+$"_L1);
    cleanName.remove(hangHyphen);

    // Generate an 8-character MD5 hash of the canonical path to the .exe (for uniqueness)
    QByteArray hashBytes = QCryptographicHash::hash(fileInfo.canonicalFilePath().toUtf8(), QCryptographicHash::Md5).toHex();
    QString pathHash = QString::fromUtf8(hashBytes.left(8));

    return QStringLiteral("%1-%2").arg(cleanName, pathHash);
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
