#include "executable_file.hpp"

#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTemporaryFile>

#include "prefix_model.hpp"

using namespace kisel;

ExecutableFile::ExecutableFile(const QString& path, QObject* parent)
    : QObject(parent)
    , m_fileInfo(path)
    , m_needUpdateIcon(!path.isEmpty())
{
}

void ExecutableFile::setPrefixName(const QString& prefixName)
{
    setPrefix(PREFIX_MODEL->forName(prefixName));
}

void ExecutableFile::setPrefix(Prefix* prefix)
{
    if (prefix == nullptr) {
        m_prefix = PREFIX_MODEL->defaultPrefix();
        qDebug() << "Unable to set prefix, default prefix is ​​used";
    } else {
        m_prefix = prefix;
    }
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

Prefix* ExecutableFile::prefix() const
{
    return m_prefix;
}

const QIcon& ExecutableFile::icon()
{
    if (m_needUpdateIcon) {
        loadIcon();
        m_needUpdateIcon = false;
    }
    return m_icon;
}

bool ExecutableFile::isValid() const
{
    return m_fileInfo.exists() && m_fileInfo.suffix().toLower() == "exe";
}

void ExecutableFile::loadIcon()
{
    if (!isValid()) {
        return;
    }

    QString bestIconGroupName = findBestIconGroupName();

    QTemporaryFile tempIconGroupFile(QDir::tempPath() % "/" % name() % "_icon_group_XXXXXX.ico");
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
    process.start("wrestool", { "-l", "-t14", path() }); // t14 - icon group

    if (!process.waitForFinished() || process.exitCode() != 0) {
        qCritical() << "Error while searching for best icon name using wrestool";
        return { };
    }

    QString output = process.readAllStandardOutput();
    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    static QRegularExpression regex(R"(--name='?(?<name>[^'\s]+)'?.*size=(?<size>\d+))");

    QString bestIconName = "";
    int maxIconSize = -1;
    for (QStringView line : lines) {
        QRegularExpressionMatch match = regex.matchView(line);
        if (match.hasMatch()) {
            int size = match.captured("size").toInt();
            if (size > maxIconSize) {
                bestIconName = match.captured("name");
                maxIconSize = size;
            }
        }
    }

    return bestIconName;
}

bool ExecutableFile::extractIconGroup(const QString& groupName, const QString& outputPath) const
{
    QProcess process;
    process.start("wrestool", { "-x", QString("--name=%1").arg(groupName), path(), "-o", outputPath });

    if (!process.waitForFinished() || process.exitCode() != 0) {
        qCritical() << "Error while extracting icon group from exe using wrestool";
        return false;
    }

    return true;
}

void ExecutableFile::createShortcut(
    QString shortcutName,
    ShortcutDestination shortcutDest,
    const QString& category)
{
    if (!isValid() || !m_prefix) {
        return;
    }

    QDir iconsDir(m_prefix->path() % "/.kisel/icons/");
    QString iconPath = saveIconWithHashName(iconsDir);
    if (iconPath.isEmpty()) {
        qWarning() << "Failed to save icon for shortcut";
    }

    QDir shortcutDestDir;
    if (shortcutDest == ShortcutDestination::Menu) {
        shortcutDestDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    } else if (shortcutDest == ShortcutDestination::Desktop) {
        shortcutDestDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    } else {
        qCritical() << "Invalid shortcut destination";
        return;
    }

    QFile desktopFile(shortcutDestDir.filePath(baseName() % ".desktop"));
    if (!desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to open shortcut file for writing:" << desktopFile.fileName();
        return;
    }

    if (shortcutName.isEmpty()) {
        shortcutName = baseName();
    }

    QTextStream stream(&desktopFile);
    stream << "[Desktop Entry]\n";
    stream << "Type=Application\n";
    stream << QString("Name=%1\n").arg(shortcutName);
    stream << QString("Exec=kisel -p \"%1\" \"%2\"\n").arg(m_prefix->name(), path());
    stream << QString("Icon=%1\n").arg(iconPath);
    stream << QString("Categories=%1\n").arg(category);
    stream << "StartupNotify=true\n";
    stream << "Terminal=false\n";

    desktopFile.close();

    QFileDevice::Permissions permissions = desktopFile.permissions();
    permissions |= QFileDevice::ExeUser;

    if (!desktopFile.setPermissions(permissions)) {
        qWarning() << "Failed to make .desktop file executable";
    }
}

QString ExecutableFile::saveIconWithHashName(const QDir& outputDir)
{
    if (m_icon.isNull()) {
        return { };
    }

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);

    QList<QSize> availableSizes = m_icon.availableSizes();
    QSize targetSize(256, 256);
    if (!availableSizes.isEmpty()) {
        targetSize = availableSizes.last();
    }

    const QPixmap& pixmap = m_icon.pixmap(targetSize);

    if (!pixmap.save(&buffer, "PNG")) {
        return { };
    }

    QByteArray hashBytes = QCryptographicHash::hash(bytes, QCryptographicHash::Md5);
    QString hashString = QString::fromLatin1(hashBytes.toHex());

    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            qCritical() << "Unable to create prefix icons directory:" << outputDir.path();
            return { };
        }
    }

    QString filePath = outputDir.filePath(name() % "_" % hashString % ".png");
    if (pixmap.save(filePath, "PNG")) {
        return filePath;
    }

    return { };
}
