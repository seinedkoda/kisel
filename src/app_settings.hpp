#pragma once

#include <QDir>
#include <QSettings>

namespace kisel {
#define APP_SETTINGS AppSettings::instance()
#define APP_DATA_DIR AppSettings::instance()->appDataDir()
#define PREFIXES_DIR AppSettings::instance()->prefixesDir()
#define CTS_DIR_LIST AppSettings::instance()->ctsDirList()

class AppSettings : public QSettings {
    Q_OBJECT

public:
    static AppSettings* instance();

    static const QDir& appDataDir();
    static const QDir& prefixesDir();
    static const QList<QDir>& ctsDirList();

    void setLocale(const QString& localeName);
    [[nodiscard]] QString locale() const;

    static bool isFlatpak();

    void setUseIndividualPrefix(bool useIndividualPrefix);
    [[nodiscard]] bool useIndividualPrefix() const;

    void setDefaultPrefixPath(const QString& prefixPath);
    [[nodiscard]] QString defaultPrefixPath() const;
    [[nodiscard]] QString defaultPrefixName() const;

    void setDefaultCtPath(const QString& ctPath);
    [[nodiscard]] QString defaultCtPath() const;

    void setRuntimeAutoUpdate(bool enable);
    [[nodiscard]] bool runtimeAutoUpdate() const;

    static const QDir& steamDir();
    static bool steamExists();

    void setUseSystemUMU(bool use);
    [[nodiscard]] bool useSystemUMU() const;
    [[nodiscard]] QString umuPath() const;

    static const QString& winetricksPath();

    static const QString& mangoHudPath();

    static const QString& obsVkCapturePath();

private:
    explicit AppSettings(QObject* parent = nullptr);
};
}
