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
    QString locale();

    void setUseIndividualPrefix(bool useIndividualPrefix);
    bool useIndividualPrefix();

    void setDefaultPrefixPath(const QString& prefixPath);
    QString defaultPrefixPath();
    QString defaultPrefixName();

    void setDefaultCtPath(const QString& ctPath);
    QString defaultCtPath();

    void setRuntimeAutoUpdate(bool enable);
    bool runtimeAutoUpdate();

    static const QDir& steamDir();
    static bool steamExists();

    static const QString& umuPath();
    static const QString& winetricksPath();

private:
    explicit AppSettings(QObject* parent = nullptr);
};
}
