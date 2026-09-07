#include "executable_file.hpp"

#include <QCryptographicHash>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTemporaryFile>

using namespace Qt::StringLiterals;
using namespace kisel;

ExecutableFile::ExecutableFile(const QString& path, QObject* parent)
    : QObject(parent)
{
    setPath(path);
}

void ExecutableFile::setPath(const QString& newPath)
{
    m_fileInfo.setFile(newPath);
    setIdFromPath();
    loadIcon();
}

QString ExecutableFile::id() const
{
    return m_id;
}

QString ExecutableFile::path() const
{
    return m_fileInfo.absoluteFilePath();
}

QString ExecutableFile::dirPath() const
{
    return m_fileInfo.dir().absolutePath();
}

QString ExecutableFile::name() const
{
    return m_fileInfo.fileName();
}

QString ExecutableFile::baseName() const
{
    return m_fileInfo.baseName();
}

bool ExecutableFile::isValid() const
{
    const QString& suffix = m_fileInfo.suffix().toLower();
    return m_fileInfo.exists()
        && (suffix == "exe"_L1 || suffix == "msi"_L1 || suffix == "bat"_L1 || suffix == "cmd"_L1);
}

bool ExecutableFile::isMsi() const
{
    return m_fileInfo.suffix().toLower() == "msi"_L1;
}

bool ExecutableFile::isCmd() const
{
    const QString& suffix = m_fileInfo.suffix().toLower();
    return suffix == "bat"_L1 || suffix == "cmd"_L1;
}

void ExecutableFile::setIdFromPath()
{
    if (!m_fileInfo.exists()) {
        m_id.clear();
        return;
    }

    QString cleanName = m_fileInfo.baseName().toLower();

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
    QByteArray hashBytes = QCryptographicHash::hash(m_fileInfo.canonicalFilePath().toUtf8(), QCryptographicHash::Md5).toHex();
    QString pathHash = QString::fromUtf8(hashBytes.left(8));

    m_id = QStringLiteral("%1-%2").arg(cleanName, pathHash);
}

const QIcon& ExecutableFile::icon() const
{
    return m_icon;
}

void ExecutableFile::loadIcon()
{
    if (!isValid()) {
        m_icon = QIcon();
        return;
    }

    QString bestIconGroupName = findBestIconGroupName();

    QTemporaryFile tempIconGroupFile(QDir::tempPath() % "/"_L1 % name() % "_icon_group_XXXXXX.ico"_L1);
    if (!tempIconGroupFile.open()) {
        qCritical() << "Failed to open temporary icon group file";
        return;
    }

    if (!extractIconGroup(bestIconGroupName, tempIconGroupFile.fileName())) {
        return;
    }

    // Stores a group of icons and automatically selects the highest quality one
    m_icon = QIcon(tempIconGroupFile.fileName());
}

QString ExecutableFile::findBestIconGroupName() const
{
    QProcess process;
    process.start("wrestool"_L1, { "-l"_L1, "-t14"_L1, path() }); // t14 - icon group

    if (!process.waitForFinished() || process.exitCode() != 0) {
        qCritical() << "Error while searching for best icon name using wrestool:" << process.readAllStandardError();
        return { };
    }

    QString output = QString::fromUtf8(process.readAllStandardOutput());
    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    static const QRegularExpression regex(R"(--name='?(?<name>[^'\s]+)'?.*size=(?<size>\d+))"_L1);

    QString bestIconName = "";
    int maxIconSize = -1;
    for (const QString& line : lines) {
        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            int size = match.captured("size"_L1).toInt();
            if (size > maxIconSize) {
                bestIconName = match.captured("name"_L1);
                maxIconSize = size;
            }
        }
    }

    return bestIconName;
}

bool ExecutableFile::extractIconGroup(const QString& groupName, const QString& outputPath) const
{
    QProcess process;
    process.start("wrestool"_L1, { "-x"_L1, QStringLiteral("--name=%1").arg(groupName), path(), "-o"_L1, outputPath });

    if (!process.waitForFinished() || process.exitCode() != 0) {
        qCritical() << "Error while extracting icon group from exe using wrestool:" << process.readAllStandardError();
        return false;
    }

    return true;
}
