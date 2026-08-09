#include "executable_file.hpp"

#include <QBuffer>
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
    , m_fileInfo(path)
    , m_needUpdateIcon(!path.isEmpty())
{
}

void ExecutableFile::setPath(const QString& newPath)
{
    m_fileInfo.setFile(newPath);
    m_needUpdateIcon = true;
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

const QIcon& ExecutableFile::icon()
{
    if (m_needUpdateIcon) {
        loadIcon();
        m_needUpdateIcon = false;
    }
    return m_icon;
}

void ExecutableFile::loadIcon()
{
    if (!isValid()) {
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

void ExecutableFile::createShortcut(
    const Prefix& prefix,
    QString shortcutName,
    ShortcutDestination shortcutDest,
    const QString& category) const
{
    if (!isValid()) {
        return;
    }

    const QDir iconsDir(prefix.dir().filePath(".kisel/icons/"_L1));
    QString iconPath = saveIconWithHashName(iconsDir);
    if (iconPath.isEmpty()) {
        qWarning() << "Failed to save icon for shortcut";
    }

    QStandardPaths::StandardLocation outputLocation { };
    if (shortcutDest == Menu) {
        outputLocation = QStandardPaths::ApplicationsLocation;
    } else if (shortcutDest == Desktop) {
        outputLocation = QStandardPaths::DesktopLocation;
    } else {
        qCritical() << "Invalid shortcut destination";
        return;
    }

    const QString destDirPath = QStandardPaths::writableLocation(outputLocation);
    if (destDirPath.isEmpty()) {
        qCritical() << "Could not determine writable location for shortcut";
        return;
    }

    if (!QDir().mkpath(destDirPath)) {
        qCritical() << "Failed to create destination directory:" << destDirPath;
        return;
    }

    const QDir shortcutDestDir(destDirPath);
    const QString desktopFilePath = shortcutDestDir.filePath(baseName() % ".desktop"_L1);
    QFile desktopFile(desktopFilePath);
    if (!desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to open shortcut file for writing:" << desktopFilePath;
        return;
    }

    if (shortcutName.isEmpty()) {
        shortcutName = baseName();
    }

    // Escape characters
    auto escapeExecArg = [](QString str) {
        str.replace(u'\\', R"(\\)"_L1);
        str.replace(u'"', R"(\")"_L1);
        str.replace(u'%', R"(%%)"_L1);
        return str;
    };

    const QString escapedPrefixName = escapeExecArg(prefix.name());
    const QString escapedExePath = escapeExecArg(path());

    QTextStream stream(&desktopFile);
    stream << "[Desktop Entry]\n"_L1;
    stream << "Type=Application\n"_L1;
    stream << "Name="_L1 << shortcutName << u'\n';
    stream << "Exec=kisel -p \""_L1 << escapedPrefixName << "\" \""_L1 << escapedExePath << "\"\n"_L1;
    stream << "Icon="_L1 << iconPath << u'\n';
    stream << "Categories="_L1 << category << ";\n"_L1;
    stream << "StartupNotify=true\n"_L1;
    stream << "Terminal=false\n"_L1;

    desktopFile.close();

    const QFileDevice::Permissions permissions = QFile::permissions(desktopFilePath) | QFileDevice::ExeUser;
    if (!QFile::setPermissions(desktopFilePath, permissions)) {
        qWarning() << "Failed to make .desktop file executable:" << desktopFilePath;
    }
}

QString ExecutableFile::saveIconWithHashName(const QDir& outputDir) const
{
    if (m_icon.isNull()) {
        return { };
    }

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);

    const QSize size = m_icon.actualSize(QSize(256, 256));
    const QPixmap& pixmap = m_icon.pixmap(size);

    if (!pixmap.save(&buffer, "PNG")) {
        return { };
    }

    const QByteArray hashBytes = QCryptographicHash::hash(bytes, QCryptographicHash::Md5);
    QString hashString = QString::fromLatin1(hashBytes.toHex());

    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            qCritical() << "Unable to create prefix icons directory:" << outputDir.path();
            return { };
        }
    }

    QString filePath = outputDir.filePath(name() % "_"_L1 % hashString % ".png"_L1);
    if (pixmap.save(filePath, "PNG")) {
        return filePath;
    }

    return { };
}
