#include "shortcuts_utils.hpp"

#include <QBuffer>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QTemporaryFile>

#include "core/appsettings/app_settings.hpp"
#include "core/executablefile/executable_file.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

QString saveHashedExeIconInPrefix(const ExecutableFile* exeFile, QSize iconSize, const Prefix* prefix)
{
    const QIcon& icon = exeFile->icon();
    if (icon.isNull()) {
        return { };
    }

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);

    if (iconSize.isNull()) {
        iconSize = QSize(256, 256);
    }

    const QSize size = icon.actualSize(iconSize);
    const QPixmap& pixmap = icon.pixmap(size);

    if (!pixmap.save(&buffer, "PNG")) {
        return { };
    }

    const QByteArray hashBytes = QCryptographicHash::hash(bytes, QCryptographicHash::Md5);
    QString hashString = QString::fromLatin1(hashBytes.toHex());

    const QDir iconsDir(prefix->dir().filePath(".kisel/icons/"_L1));
    if (!iconsDir.exists() && !iconsDir.mkpath(".")) {
        qCritical() << "Failed to create prefix icons directory:" << iconsDir.path();
        return { };
    }

    QString filePath = iconsDir.filePath(exeFile->name() % "_"_L1 % hashString % ".png"_L1);
    if (pixmap.save(filePath, "PNG")) {
        return filePath;
    }

    return { };
}

QList<QDir> shortcutDirs(ShortcutLocations locations)
{
    QList<QDir> dirs;

    if (locations.testFlag(ShortcutLocation::Desktop)) {
        dirs.append(QDir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)));
    }

    if (locations.testFlag(ShortcutLocation::Menu)) {
        if (APP_SETTINGS->isFlatpak()) {
            dirs.append(QDir(QDir::homePath() % "/.local/share/applications"_L1)); // or "--filesystem=xdg-data/applications"
        }
        dirs.append(QDir(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)));
    }

    return dirs;
}

QString escapeExecArg(QString arg)
{
    arg.replace(u'\\', R"(\\)"_L1);
    arg.replace(u'"', R"(\")"_L1);
    arg.replace(u'%', R"(%%)"_L1);
    return arg;
}

void kisel::createShortcut(
    const ExecutableFile* exeFile,
    const Prefix* prefix,
    ShortcutLocations locations,
    QString shortcutName,
    QSize iconSize,
    const QString& category)
{
    if (!exeFile->isValid()) {
        return;
    }

    QString iconPath = saveHashedExeIconInPrefix(exeFile, iconSize, prefix);
    if (iconPath.isEmpty()) {
        qWarning() << "Failed to save icon for shortcut";
    }

    const QString shortcutFileName = exeFile->baseName() % ".kisel.desktop"_L1;
    QTemporaryFile tempDesktopFile(shortcutFileName);
    if (!tempDesktopFile.open()) {
        qCritical() << "Failed to open temporary shortcut file for writing";
        return;
    }

    if (shortcutName.isEmpty()) {
        shortcutName = exeFile->baseName();
    }

    const QString escapedPrefixName = escapeExecArg(prefix->name());
    const QString escapedExePath = escapeExecArg(exeFile->path());

    QTextStream stream(&tempDesktopFile);
    stream << "[Desktop Entry]\n"_L1;
    stream << "Type=Application\n"_L1;
    stream << "Name="_L1 << shortcutName << u'\n';
    if (APP_SETTINGS->isFlatpak()) {
        stream << "Exec=flatpak run --file-forwarding io.github.seinedkoda.kisel -p \""_L1 << escapedPrefixName << "\" @@ \""_L1 << escapedExePath << "\" @@\n"_L1;
    } else {
        stream << "Exec=kisel -p \""_L1 << escapedPrefixName << "\" \""_L1 << escapedExePath << "\"\n"_L1;
    }
    stream << "Icon="_L1 << iconPath << u'\n';
    stream << "Categories="_L1 << category << ";\n"_L1;
    stream << "StartupNotify=true\n"_L1;
    stream << "Terminal=false\n"_L1;

    tempDesktopFile.close();

    if (!tempDesktopFile.setPermissions(tempDesktopFile.permissions() | QFileDevice::ExeUser)) {
        qWarning() << "Failed to make .desktop file executable";
    }

    const QList<QDir> dirs = shortcutDirs(locations);
    for (const QDir& dir : dirs) {
        if (!dir.exists() && !dir.mkpath(".")) {
            qCritical() << "Failed to create shortcut directory:" << dir.path();
            continue;
        }

        QString destPath = dir.filePath(shortcutFileName);
        if (!tempDesktopFile.copy(destPath)) {
            qCritical() << "Failed to copy file to:" << destPath;
        }
    }
}
