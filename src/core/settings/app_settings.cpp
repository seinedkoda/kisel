#include "app_settings.hpp"

#include <QApplication>
#include <QApplicationStatic>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>
#include <utility>

using namespace Qt::StringLiterals;
using namespace kisel;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Q_APPLICATION_STATIC(AppSettings, g_appSettings)

AppSettings::AppSettings(QObject* parent)
    : BaseSettings(appConfigPath(), parent)
{
    upgradeOldData();

    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << ":/thirdparty");

    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        QIcon::setFallbackThemeName("Kisel-Papirus-Dark");
    } else {
        QIcon::setFallbackThemeName("Kisel-Papirus-Light");
    }

    int iconTheme = iconThemeType();
    if (iconTheme == 1) {
        QIcon::setThemeName("Kisel-Papirus-Light");
    } else if (iconTheme == 2) {
        QIcon::setThemeName("Kisel-Papirus-Dark");
    }

    m_appDirs = { prefixesDir(), ctsDirList().first() };

    loadLanguageMap();
}

AppSettings* AppSettings::instance()
{
    return g_appSettings;
}

void AppSettings::createAppDirectories()
{
    for (const auto& dir : std::as_const(m_appDirs)) {
        if (!dir.exists() && !dir.mkpath(".")) {
            qCritical() << "Failed to create important directory:" << dir.absolutePath();
        }
    }
}

void AppSettings::upgradeOldData()
{
    // 1.4 -> 1.5
    if (contains("defaultPrefix"_L1)) {
        qInfo() << "\"defaultPrefix\" upgrade";
        QString defaultPrefix = value("defaultPrefix"_L1).toString();
        setDefaultPrefixName(QFileInfo(defaultPrefix).fileName());
        remove("defaultPrefix"_L1);
    }

    if (contains("individualPrefix"_L1)) {
        qInfo() << "\"individualPrefix\" upgrade";
        bool individualPrefix = value("individualPrefix"_L1).toBool();
        setPrefixType(individualPrefix ? PrefixType::Individual : PrefixType::Shared);
        remove("individualPrefix"_L1);
    }
}

void AppSettings::loadLanguageMap()
{
    const QStringList translationFiles = QDir(":/i18n"_L1).entryList({ "kisel_*.qm"_L1 });
    for (const QString& fileName : translationFiles) {
        const QString code = fileName.mid(6, fileName.lastIndexOf('.') - 6);
        const QString name = QLocale(code).nativeLanguageName();
        m_languageMap.insert(name, code);
    }
}

const QString& AppSettings::appConfigPath()
{
    static QString appConfigFilePath = QDir::home().filePath(".config/kisel/kisel.conf"_L1);
    return appConfigFilePath;
}

const QDir& AppSettings::appDataDir()
{
    static QDir dir(QDir::home().filePath(".local/share/kisel/"_L1));
    return dir;
}

const QString& AppSettings::logFilePath()
{
    static QString logFilePath = appDataDir().filePath("kisel.log"_L1);
    return logFilePath;
}

const QDir& AppSettings::appPrefixesDir()
{
    static QDir dir(appDataDir().filePath("prefixes/"_L1));
    return dir;
}

QString AppSettings::appDefaultPrefixName()
{
    return "Default"_L1;
}

QString AppSettings::portablePrefixName()
{
    return ".kisel-prefix"_L1;
}

const QList<QDir>& AppSettings::ctsDirList()
{
    static QList<QDir> list {
        appDataDir().filePath("compatibilitytools.d/"_L1),
        steamDir().filePath("compatibilitytools.d/"_L1),
    };
    return list;
}

void AppSettings::setLanguage(const QString& languageName)
{
    if (!m_languageMap.contains(languageName)) {
        return;
    }

    const QString localeName = m_languageMap.value(languageName);
    installLocale(localeName);
    saveLocale(localeName);
}

QString AppSettings::language() const
{
    return m_currentLanguageName;
}

void AppSettings::installLocale(QString localeName)
{
    if (localeName.isEmpty()) {
        localeName = locale();
    }

    QLocale locale(localeName);
    if (m_qTranslator.load(locale, "kisel"_L1, "_"_L1, ":/i18n"_L1)) {
        if (qApp->installTranslator(&m_qTranslator)) {
            m_currentLanguageName = locale.nativeLanguageName();
        }
    }
}

void AppSettings::saveLocale(const QString& localeName)
{
    setValue("locale"_L1, localeName);
}

QString AppSettings::locale() const
{
    return value("locale"_L1, QLocale::system().name()).toString();
}

QStringList AppSettings::languagesList() const
{
    return m_languageMap.keys();
}

bool AppSettings::isFlatpak()
{
    return QProcessEnvironment::systemEnvironment().contains("FLATPAK_ID"_L1);
}

void AppSettings::setIconThemeType(int iconThemeType)
{
    setValue("iconThemeType"_L1, iconThemeType);
}

int AppSettings::iconThemeType()
{
    // 0 - System theme
    // 1 - Kisel-Papirus-Light
    // 2 - Kisel-Papirus-Dark
    return value("iconThemeType"_L1, 0).toInt();
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

void AppSettings::setInstantRunEnabled(bool enabled) {
    setValue("instantRun"_L1, enabled);
}

bool AppSettings::instantRunEnabled() {
    return value("instantRun"_L1, false).toBool();
}

void AppSettings::setPrefixType(PrefixType type)
{
    setValue("prefixType"_L1, type);
}

AppSettings::PrefixType AppSettings::prefixType() const
{
    return value("prefixType"_L1, PrefixType::Shared).value<PrefixType>();
}

void AppSettings::setPrefixesDir(const QString& dirPath)
{
    setValue("prefixesDir"_L1, dirPath);
}

QDir AppSettings::prefixesDir() const
{
    return { value("prefixesDir"_L1, appPrefixesDir().path()).toString() };
}

void AppSettings::setDefaultPrefixName(const QString& prefixName)
{
    setValue("defaultPrefixName"_L1, prefixName);
}

QString AppSettings::defaultPrefixName() const
{
    return value("defaultPrefixName"_L1, appDefaultPrefixName()).toString();
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
    static QDir steamDir(QDir::home().filePath(".local/share/Steam"_L1));
    return steamDir;
}

bool AppSettings::steamDirExists()
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
        return "/app/lib/kisel/umu-run"_L1;
    }

    if (!useSystemUMU() && QFileInfo::exists("/usr/lib/kisel/umu-run"_L1)) {
        return "/usr/lib/kisel/umu-run"_L1;
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

const QString& AppSettings::gamescopePath()
{
    static QString gamescopePath = QStandardPaths::findExecutable("gamescope"_L1);
    return gamescopePath;
}

const QString& AppSettings::obsVkCapturePath()
{
    static QString obsVkCapturePath = QStandardPaths::findExecutable("obs-gamecapture"_L1);
    return obsVkCapturePath;
}
