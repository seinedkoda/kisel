#include "app_settings.hpp"

#include <QFileInfo>
#include <QStandardPaths>

using namespace Qt::StringLiterals;
using namespace kisel;

AppSettings::AppSettings(QObject* parent)
    : QSettings(QDir::homePath() % "/.config/kisel/kisel.conf"_L1, QSettings::IniFormat, parent)
{
}

AppSettings* AppSettings::instance()
{
    static AppSettings instance;
    return &instance;
}

const QDir& AppSettings::appDataDir()
{
    static QDir dir(QDir::homePath() % "/.local/share/kisel/"_L1);
    return dir;
}

const QDir& AppSettings::prefixesDir()
{
    static QDir dir(appDataDir().filePath("prefixes/"_L1));
    return dir;
}

const QList<QDir>& AppSettings::ctsDirList()
{
    static QList<QDir> list {
        appDataDir().filePath("compatibilitytools.d/"_L1),
        appDataDir().filePath(QDir::homePath() % "/.steam/steam/compatibilitytools.d/"_L1)
    };
    return list;
}

void AppSettings::setLocale(const QString& localeName)
{
    setValue("locale"_L1, localeName);
}

QString AppSettings::locale()
{
    return value("locale"_L1, QLocale::system().name()).toString();
}

void AppSettings::setUseIndividualPrefix(bool useIndividualPrefix) {
    setValue("individualPrefix"_L1, useIndividualPrefix);
}

bool AppSettings::useIndividualPrefix() {
    return value("individualPrefix"_L1, false).toBool();
}

void AppSettings::setDefaultPrefixPath(const QString& prefixPath)
{
    setValue("defaultPrefix"_L1, prefixPath);
}

QString AppSettings::defaultPrefixPath()
{
    static QString defaultPrefixPath = appDataDir().filePath("prefixes/Default/"_L1);
    return value("defaultPrefix"_L1, defaultPrefixPath).toString();
}

QString AppSettings::defaultPrefixName()
{
    const QString defaultPrefixName = QFileInfo(defaultPrefixPath()).fileName();
    return defaultPrefixName.isEmpty() ? "Default"_L1 : defaultPrefixName;
}

void AppSettings::setDefaultCtPath(const QString& ctPath)
{
    setValue("defaultCt"_L1, ctPath);
}

QString AppSettings::defaultCtPath()
{
    return value("defaultCt"_L1).toString();
}

void AppSettings::setRuntimeAutoUpdate(bool enable)
{
    setValue("runtimeAutoUpdate"_L1, enable);
}

bool AppSettings::runtimeAutoUpdate()
{
    return value("runtimeAutoUpdate"_L1, true).toBool();
}

const QDir& AppSettings::steamDir() {
    static QDir steamDir(QDir::homePath() % "/.local/share/Steam"_L1);
    return steamDir;
}

bool AppSettings::steamExists() {
    static bool steamExists = steamDir().exists();
    return steamExists;
}

const QString& AppSettings::winetricksPath() {
    static QString winetricksPath = QStandardPaths::findExecutable("winetricks"_L1);
    return winetricksPath;
}

const QString& AppSettings::umuPath() {
    static QString umuPath = QStandardPaths::findExecutable("umu-run"_L1);
    return umuPath;
}