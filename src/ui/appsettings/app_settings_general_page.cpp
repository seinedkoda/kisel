#include "app_settings_general_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStyleFactory>

#include "core/appsettings/app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

AppSettingsGeneralPage::AppSettingsGeneralPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* languageLabel = new QLabel(tr("Language"), this);
    layout->addWidget(languageLabel);

    auto* languageComboBox = new QComboBox(this);
    languageComboBox->addItems(APP_SETTINGS->languagesList());
    languageComboBox->setCurrentText(APP_SETTINGS->language());
    connect(languageComboBox, &QComboBox::currentTextChanged, this, [](const QString& languageName) {
        APP_SETTINGS->setLanguage(languageName);
    });
    layout->addWidget(languageComboBox);

    auto* styleLabel = new QLabel(tr("Style"), this);
    layout->addWidget(styleLabel);

    auto* styleComboBox = new QComboBox(this);
    static QStringList styles = QStyleFactory::keys();
    styleComboBox->addItems(styles);
    const QString savedStyle = APP_SETTINGS->styleName();
    if (styles.contains(savedStyle)) {
        styleComboBox->setCurrentText(savedStyle);
    } else {
        styleComboBox->setCurrentText(QStringLiteral("Fusion"));
    }
    connect(styleComboBox, &QComboBox::currentTextChanged, this, [](const QString& styleName) {
        APP_SETTINGS->setStyleName(styleName);
    });
    layout->addWidget(styleComboBox);

    auto* iconThemeTypeLabel = new QLabel(tr("Icon theme type"), this);
    layout->addWidget(iconThemeTypeLabel);

    auto* iconThemeTypeComboBox = new QComboBox(this);
    iconThemeTypeComboBox->addItems({ tr("System"), "Kisel-Papirus-Light"_L1, "Kisel-Papirus-Dark"_L1 });
    iconThemeTypeComboBox->setCurrentIndex(APP_SETTINGS->iconThemeType());
    connect(iconThemeTypeComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setIconThemeType(index);
    });
    layout->addWidget(iconThemeTypeComboBox);

    auto* topLoggingLine = new QFrame(this);
    topLoggingLine->setFrameShape(QFrame::HLine);
    layout->addWidget(topLoggingLine);

    auto* loggingCheckBox = new QCheckBox(tr("Logging"), this);
    loggingCheckBox->setChecked(APP_SETTINGS->loggingEnabled());
    layout->addWidget(loggingCheckBox);

    auto* openLogFileButton = new QPushButton(QIcon::fromTheme("document-open-recent"), tr("Open log file"), this);
    openLogFileButton->setEnabled(APP_SETTINGS->loggingEnabled());
    layout->addWidget(openLogFileButton);

    connect(loggingCheckBox, &QCheckBox::clicked, this, [openLogFileButton](bool checked) {
        APP_SETTINGS->setLoggingEnabled(checked);
        openLogFileButton->setEnabled(checked);
    });

    connect(openLogFileButton, &QPushButton::clicked, this, [this]() {
        if (QFileInfo::exists(APP_SETTINGS->logFilePath())) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(APP_SETTINGS->logFilePath()));
        } else {
            QMessageBox::information(this, tr("Unable to open"), tr("The log file does not exist"));
        }
    });
}
