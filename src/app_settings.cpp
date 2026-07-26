#include "app_settings.hpp"

#include <QFileInfo>
#include <QStandardPaths>

using namespace kisel;

AppSettings::AppSettings(QObject* parent)
    : QSettings(QDir::homePath() % "/.config/kisel/kisel.conf", QSettings::IniFormat, parent)
{
}

AppSettings* AppSettings::instance()
{
    static AppSettings instance;
    return &instance;
}

const QDir& AppSettings::appDataDir()
{
    static QDir dir(QDir::homePath() % "/.local/share/kisel/");
    return dir;
}

const QDir& AppSettings::prefixesDir()
{
    static QDir dir(appDataDir().filePath("prefixes/"));
    return dir;
}

const QList<QDir>& AppSettings::ctsDirList()
{
    static QList<QDir> list {
        appDataDir().filePath("compatibilitytools.d/"),
        appDataDir().filePath(QDir::homePath() % "/.steam/steam/compatibilitytools.d/")
    };
    return list;
}

void AppSettings::setLocale(const QString& localeName)
{
    setValue("locale", localeName);
}

QString AppSettings::locale()
{
    return value("locale", QLocale::system().name()).toString();
}

void AppSettings::setDefaultPrefixPath(const QString& prefixPath)
{
    setValue("defaultPrefix", prefixPath);
}

QString AppSettings::defaultPrefixPath()
{
    static QString defaultPrefixPath = appDataDir().filePath("prefixes/Default/");
    return value("defaultPrefix", defaultPrefixPath).toString();
}

QString AppSettings::defaultPrefixName()
{
    const QString defaultPrefixName = QFileInfo(defaultPrefixPath()).fileName();
    return defaultPrefixName.isEmpty() ? "Default" : defaultPrefixName;
}

void AppSettings::setDefaultCtPath(const QString& ctPath)
{
    setValue("defaultCt", ctPath);
}

QString AppSettings::defaultCtPath()
{
    return value("defaultCt").toString();
}

void AppSettings::setRuntimeAutoUpdate(bool enable)
{
    setValue("runtimeAutoUpdate", enable);
}

bool AppSettings::runtimeAutoUpdate()
{
    return value("runtimeAutoUpdate", true).toBool();
}

const QDir& AppSettings::steamDir() {
    static QDir steamDir(QDir::homePath() % "/.local/share/Steam");
    return steamDir;
}

bool AppSettings::steamExists() {
    static bool steamExists = steamDir().exists();
    return steamExists;
}

const QString& AppSettings::winetricksPath() {
    static QString winetricksPath = QStandardPaths::findExecutable("winetricks");
    return winetricksPath;
}