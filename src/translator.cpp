#include "translator.hpp"

#include <QCoreApplication>
#include <QDir>

#include "app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

Translator::Translator(QObject* parent)
    : QObject(parent)
{
    const QStringList translationFiles = QDir(":/i18n"_L1).entryList({ "kisel_*.qm"_L1 });
    for (const QString& fileName : translationFiles) {
        const QString code = fileName.mid(6, fileName.lastIndexOf('.') - 6);
        const QString name = QLocale(code).nativeLanguageName();
        m_languageMap.insert(name, code);
    }
}

Translator* Translator::instance()
{
    static Translator instance;
    return &instance;
}

bool Translator::setLocale(const QString& localeName)
{
    QLocale locale(localeName);
    if (m_qtranslator.load(locale, "kisel"_L1, "_"_L1, ":/i18n"_L1)) {
        if (qApp->installTranslator(&m_qtranslator)) {
            m_currentLocale = locale;
            m_currentLanguageName = locale.nativeLanguageName();
            return true;
        }
        return false;
    }
    return false;
}

void Translator::saveLanguage(const QString& languageName)
{
    if (!m_languageMap.contains(languageName)) {
        return;
    }

    const QString localeName = m_languageMap.value(languageName);
    if (setLocale(localeName)) {
        APP_SETTINGS->setLocale(localeName);
    }
}

void Translator::setLocaleFromSettings()
{
    const QString localeName = APP_SETTINGS->locale();
    setLocale(localeName);
}

QLocale Translator::currentLocale() const
{
    return m_currentLocale;
}

QString Translator::currentLanguageName() const
{
    return m_currentLanguageName;
}

QStringList Translator::languageList() const
{
    return m_languageMap.keys();
}