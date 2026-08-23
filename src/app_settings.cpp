#include "app_settings.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>

using namespace Qt::StringLiterals;
using namespace kisel;

AppSettings::AppSettings(QObject* parent)
    : QSettings(QDir::homePath() % "/.config/kisel/kisel.conf"_L1, QSettings::IniFormat, parent)
{
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << ":/icons/thirdparty");
    QIcon::setFallbackThemeName("Papirus");
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

const QString& AppSettings::logFilePath()
{
    static QString logFilePath = appDataDir().filePath("run.log");
    return logFilePath;
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

QString AppSettings::locale() const
{
    return value("locale"_L1, QLocale::system().name()).toString();
}

bool AppSettings::isFlatpak()
{
    return QProcessEnvironment::systemEnvironment().contains("FLATPAK_ID"_L1);
}

void AppSettings::setStyleName(const QString& styleName)
{
    QApplication::setStyle(QStyleFactory::create(styleName));
    setValue("style"_L1, styleName);
}

QString AppSettings::styleName()
{
    return value("style"_L1, QApplication::style()->objectName()).toString();
}

void AppSettings::applyCurrentStyle()
{
    QApplication::setStyle(QStyleFactory::create(styleName()));
}

void AppSettings::setUseIndividualPrefix(bool useIndividualPrefix)
{
    setValue("individualPrefix"_L1, useIndividualPrefix);
}

bool AppSettings::useIndividualPrefix() const
{
    return value("individualPrefix"_L1, false).toBool();
}

void AppSettings::setDefaultPrefixPath(const QString& prefixPath)
{
    setValue("defaultPrefix"_L1, prefixPath);
}

QString AppSettings::defaultPrefixPath() const
{
    static QString defaultPrefixPath = appDataDir().filePath("prefixes/Default/"_L1);
    return value("defaultPrefix"_L1, defaultPrefixPath).toString();
}

QString AppSettings::defaultPrefixName() const
{
    const QString defaultPrefixName = QFileInfo(defaultPrefixPath()).fileName();
    return defaultPrefixName.isEmpty() ? "Default"_L1 : defaultPrefixName;
}

void AppSettings::setDefaultCtPath(const QString& ctPath)
{
    setValue("defaultCt"_L1, ctPath);
}

QString AppSettings::defaultCtPath() const
{
    return value("defaultCt"_L1).toString();
}

void AppSettings::setRuntimeAutoUpdate(bool enabled)
{
    setValue("runtimeAutoUpdate"_L1, enabled);
}

bool AppSettings::runtimeAutoUpdate() const
{
    return value("runtimeAutoUpdate"_L1, true).toBool();
}

void AppSettings::setLoggingEnabled(bool enabled)
{
    if (!enabled && QFileInfo::exists(logFilePath())) {
        QFile::remove(logFilePath());
    }
    setValue("logging"_L1, enabled);
}

bool AppSettings::loggingEnabled() const
{
    return value("logging"_L1, false).toBool();
}

const QDir& AppSettings::steamDir()
{
    static QDir steamDir(QDir::homePath() % "/.local/share/Steam"_L1);
    return steamDir;
}

bool AppSettings::steamExists()
{
    static bool steamExists = steamDir().exists();
    return steamExists;
}

void AppSettings::setUseSystemUMU(bool use)
{
    setValue("useSystemUmu"_L1, use);
}

bool AppSettings::useSystemUMU() const
{
    return value("useSystemUmu"_L1, false).toBool();
}

QString AppSettings::umuPath() const
{
    if (isFlatpak()) {
        return "/app/bin/umu-run"_L1;
    }

    if (!useSystemUMU() && QFileInfo::exists("/usr/libexec/kisel/umu-run"_L1)) {
        return "/usr/libexec/kisel/umu-run"_L1;
    }

    static QString systemUmuPath = QStandardPaths::findExecutable("umu-run"_L1);
    return systemUmuPath;
}

const QString& AppSettings::winetricksPath()
{
    static QString winetricksPath = QStandardPaths::findExecutable("winetricks"_L1);
    return winetricksPath;
}

const QString& AppSettings::mangoHudPath()
{
    static QString mangoHudPath = QStandardPaths::findExecutable("mangohud"_L1);
    return mangoHudPath;
}

const QString& AppSettings::obsVkCapturePath()
{
    static QString obsVkCapturePath = QStandardPaths::findExecutable("obs-gamecapture"_L1);
    return obsVkCapturePath;
}
