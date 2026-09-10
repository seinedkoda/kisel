#include "shortcut.hpp"

#include <QFileInfo>
#include <QStandardPaths>

#include "core/appsettings/app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

Shortcut::Shortcut(const QString& filePath, QObject* parent)
    : QObject(parent)
    , m_path(filePath)
{
    m_id = QFileInfo(filePath).fileName().chopped(QStringLiteral(".kisel.desktop").length());

    static QString desktopShortcutsPath = desktopShortcutsDir().absolutePath();
    static QString menuShortcutsPath = menuShortcutsDir().absolutePath();
    if (filePath.startsWith(desktopShortcutsPath)) {
        m_location = ShortcutLocation::Desktop;
    } else if (filePath.startsWith(menuShortcutsPath)) {
        m_location = ShortcutLocation::Menu;
    }

    parseData();
}

void Shortcut::parseData()
{
    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);

    QString line;
    bool inDesktopEntry = false;

    while (stream.readLineInto(&line)) {
        line = line.trimmed();

        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        if (line.startsWith('[') && line.endsWith(']')) {
            inDesktopEntry = (line == "[Desktop Entry]");
            continue;
        }

        if (!inDesktopEntry) {
            continue;
        }

        qsizetype equalPos = line.indexOf('=');
        if (equalPos == -1) {
            continue;
        }

        const QString key = line.left(equalPos).trimmed();
        QString value = line.mid(equalPos + 1).trimmed();

        if (key == "Name"_L1) {
            m_name = value;
        } else if (key == "Icon"_L1) {
            m_icon = QIcon(value);
        }
    }
}

QString Shortcut::id() const
{
    return m_id;
}

QString Shortcut::path() const
{
    return m_path;
}

QString Shortcut::name() const
{
    return m_name;
}

ShortcutLocation Shortcut::location() const
{
    return m_location;
}

const QIcon& Shortcut::icon() const
{
    return m_icon;
}

QList<QDir> Shortcut::shortcutDirs(ShortcutLocations locations)
{
    QList<QDir> dirs;

    if (locations.testFlag(ShortcutLocation::Desktop)) {
        dirs.append(desktopShortcutsDir());
    }

    if (locations.testFlag(ShortcutLocation::Menu)) {
        dirs.append(menuShortcutsDir());
    }

    return dirs;
}

QDir Shortcut::menuShortcutsDir()
{
    if (APP_SETTINGS->isFlatpak()) {
        return { QDir::homePath() % "/.local/share/applications"_L1 }; // or "--filesystem=xdg-data/applications"
    }
    return { QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) };
}

QDir Shortcut::desktopShortcutsDir()
{
    return { QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) };
}