#pragma once

#include <QMap>
#include <QObject>
#include <QTranslator>

namespace kisel {
#define TRANSLATOR Translator::instance()

class Translator : public QObject {
    Q_OBJECT

public:
    static Translator* instance();

    void saveLanguage(const QString& languageName);
    void setLocaleFromSettings();
    QString currentLanguageName();
    QStringList languageList();

private:
    explicit Translator(QObject* parent = nullptr);

    bool setLocale(const QString& localeName);

    QTranslator m_qtranslator;
    QMap<QString, QString> m_languageMap;
    QString m_currentLanguageName;
};
}