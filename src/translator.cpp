#include "translator.hpp"

#include <QCoreApplication>
#include <QDir>

#include "app_settings.hpp"

using namespace kisel;

Translator::Translator(QObject* parent)
    : QObject(parent)
{
    const QStringList translationFiles = QDir(":/i18n").entryList(QStringList() << "kisel_*.qm");
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
    if (m_qtranslator.load(QLocale(localeName), "kisel", "_", ":/i18n")) {
        if (qApp->installTranslator(&m_qtranslator)) {
            m_currentLanguageName = QLocale(localeName).nativeLanguageName();
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

QString Translator::currentLanguageName()
{
    return m_currentLanguageName;
}

QStringList Translator::languageList()
{
    return m_languageMap.keys();
}