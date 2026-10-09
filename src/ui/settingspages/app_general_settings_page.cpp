#include "app_general_settings_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStyleFactory>

#include "core/settings/app_settings.hpp"

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

    auto* instantRunCheckBox = new QCheckBox(tr("Instant run"), this);
    instantRunCheckBox->setToolTip(tr("Instantly run an executable file in the preferred prefix if a path is specified at startup\n"
                                      "Priority:\n"
                                      "1. Existing individual prefix\n"
                                      "2. Existing portable prefix\n"
                                      "3. Prefix containing the executable file\n"
                                      "4. Default prefix type from settings"));
    instantRunCheckBox->setChecked(APP_SETTINGS->instantRunEnabled());
    connect(instantRunCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setInstantRunEnabled(checked);
    });
    layout->addWidget(instantRunCheckBox);

    auto* topUmuLine = new QFrame(this);
    topUmuLine->setFrameShape(QFrame::HLine);
    layout->addWidget(topUmuLine);

    auto* umuLabel = new QLabel("UMU"_L1, this);
    layout->addWidget(umuLabel);

    auto* umuPathComboBox = new QComboBox(this);
    if (APP_SETTINGS->isFlatpak()) {
        umuPathComboBox->addItem(tr("Built-in (Flatpak)"), false);
        umuPathComboBox->setDisabled(true);
    } else {
        umuPathComboBox->addItem(tr("Built-in"), false);
        umuPathComboBox->addItem(tr("System"), true);
        umuPathComboBox->setCurrentIndex(APP_SETTINGS->useSystemUMU() ? 1 : 0);
        connect(umuPathComboBox, &QComboBox::activated, this, [umuPathComboBox] {
            APP_SETTINGS->setUseSystemUMU(umuPathComboBox->currentData().toBool());
        });
    }
    layout->addWidget(umuPathComboBox);

    auto* runtimeAutoUpdateCheckBox = new QCheckBox(tr("Runtime auto-update"), this);
    runtimeAutoUpdateCheckBox->setChecked(APP_SETTINGS->runtimeAutoUpdate());
    connect(runtimeAutoUpdateCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setRuntimeAutoUpdate(checked);
    });
    layout->addWidget(runtimeAutoUpdateCheckBox);

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

    connect(openLogFileButton, &QPushButton::clicked, this, [this] {
        if (QFileInfo::exists(APP_SETTINGS->logFilePath())) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(APP_SETTINGS->logFilePath()));
        } else {
            QMessageBox::information(this, tr("Unable to open"), tr("The log file does not exist"));
        }
    });
}
