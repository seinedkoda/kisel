#pragma once

#include <QMap>
#include <QObject>
#include <QTranslator>
#include <QLocale>

namespace kisel {
#define TRANSLATOR Translator::instance()

class Translator : public QObject {
    Q_OBJECT

public:
    static Translator* instance();

    void saveLanguage(const QString& languageName);
    void setLocaleFromSettings();
    [[nodiscard]] QLocale currentLocale() const;
    [[nodiscard]] QString currentLanguageName() const;
    [[nodiscard]] QStringList languageList() const;

private:
    explicit Translator(QObject* parent = nullptr);

    bool setLocale(const QString& localeName);

    QTranslator m_qtranslator;
    QMap<QString, QString> m_languageMap;
    QLocale m_currentLocale;
    QString m_currentLanguageName;
};
}