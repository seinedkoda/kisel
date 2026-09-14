#include "shortcut.hpp"

#include <QFileInfo>
#include <QProcess>
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

        if (line.isEmpty() || line.startsWith(u'#')) {
            continue;
        }

        if (line.startsWith(u'[') && line.endsWith(u']')) {
            inDesktopEntry = (line == "[Desktop Entry]"_L1);
            continue;
        }

        if (!inDesktopEntry) {
            continue;
        }

        qsizetype equalPos = line.indexOf(u'=');
        if (equalPos == -1) {
            continue;
        }

        const QString key = line.left(equalPos).trimmed();
        QString value = line.mid(equalPos + 1).trimmed();

        if (key == "Name"_L1) {
            m_name = value;
        } else if (key == "Exec"_L1) {
            parseExecValue(value);
        } else if (key == "Categories"_L1) {
            m_category = value.section(';', 0, 0);
        } else if (key == "Icon"_L1) {
            m_icon = QIcon(value);
        }
    }
}

void Shortcut::parseExecValue(QStringView value)
{
    QStringList tokens = QProcess::splitCommand(value);
    if (tokens.isEmpty()) {
        return;
    }

    for (int i = 0; i < tokens.size(); ++i) {
        const QString& token = tokens.at(i);
        bool isFile = token.startsWith(u'/');

        if (!isFile) {
            if (token.startsWith("-p"_L1) || token.startsWith("--prefix"_L1)) {
                qsizetype equalPos = token.indexOf(u'=');
                if (equalPos == -1 && i + 1 < tokens.size() && !tokens.at(i + 1).startsWith('-')) {
                    // --param value
                    m_prefixName = tokens.at(i + 1);
                } else {
                    // --param=value
                    m_prefixName = token.mid(equalPos + 1);
                }
            }
        } else {
            m_exeFilePath = token;
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

QString Shortcut::prefixName() const
{
    return m_prefixName;
}

QString Shortcut::exeFilePath() const
{
    return m_exeFilePath;
}

QString Shortcut::category() const
{
    return m_category;
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