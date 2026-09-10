#pragma once

#include <QDir>
#include <QSettings>
#include <QTranslator>
#include <QVersionNumber>

namespace kisel {
#define APP_SETTINGS AppSettings::instance()
#define APP_DATA_DIR AppSettings::instance()->appDataDir()
#define PREFIXES_DIR AppSettings::instance()->prefixesDir()
#define CTS_DIR_LIST AppSettings::instance()->ctsDirList()

class AppSettings : public QSettings {
    Q_OBJECT

public:
    explicit AppSettings(QObject* parent = nullptr);
    static AppSettings* instance();

    static const QDir& appDataDir();
    static const QString& logFilePath();
    static const QDir& prefixesDir();
    static const QList<QDir>& ctsDirList();
    void createAppDirectories();

    void setLanguage(const QString& languageName);
    [[nodiscard]] QString language() const;
    void installLocale(QString localeName = "");
    [[nodiscard]] QString locale() const;
    [[nodiscard]] QStringList languagesList() const;

    static bool isFlatpak();

    static bool deviceSupportsVulkan();
    static QVersionNumber vulkanApiVersion();
    static bool deviceSupportsModernVulkan();

    void setStyleName(const QString& styleName);
    QString styleName();
    void applyCurrentStyle();

    void setUseIndividualPrefix(bool useIndividualPrefix);
    [[nodiscard]] bool useIndividualPrefix() const;

    void setDefaultPrefixPath(const QString& prefixPath);
    [[nodiscard]] QString defaultPrefixPath() const;
    [[nodiscard]] QString defaultPrefixName() const;

    void setDefaultCtPath(const QString& ctPath);
    [[nodiscard]] QString defaultCtPath() const;

    void setRuntimeAutoUpdate(bool enabled);
    [[nodiscard]] bool runtimeAutoUpdate() const;

    void setLoggingEnabled(bool enabled);
    [[nodiscard]] bool loggingEnabled() const;

    static const QDir& steamDir();
    static bool steamExists();

    void setUseSystemUMU(bool use);
    [[nodiscard]] bool useSystemUMU() const;
    [[nodiscard]] QString umuPath() const;

    static const QString& winetricksPath();

    static const QString& mangoHudPath();

    static const QString& obsVkCapturePath();

private:
    void loadLanguageMap();

    QList<QDir> m_appDirs;
    QTranslator m_qTranslator;
    QMap<QString, QString> m_languageMap;
    QString m_currentLanguageName;
};
}
