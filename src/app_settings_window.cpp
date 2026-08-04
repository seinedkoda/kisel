#include "app_settings_window.hpp"

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include "app_settings.hpp"
#include "ct_model.hpp"
#include "prefix_model.hpp"
#include "translator.hpp"

using namespace kisel;

AppSettingsWindow::AppSettingsWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Kisel — Settings"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* centralWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    auto* aboutAppBox = new QGroupBox(tr("About the program"), this);
    auto* aboutAppLayout = new QHBoxLayout(aboutAppBox);
    layout->addWidget(aboutAppBox);

    auto* logoLabel = new QLabel(this);
    logoLabel->setFixedSize(64, 64);
    QPixmap logoPixmap(":/icons/kisel.png");
    logoLabel->setPixmap(logoPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignTop);
    aboutAppLayout->addWidget(logoLabel);

    auto* aboutAppInfoWidget = new QWidget(this);
    auto* aboutAppInfoLayout = new QVBoxLayout(aboutAppInfoWidget);
    aboutAppInfoLayout->setAlignment(Qt::AlignTop);
    aboutAppLayout->addWidget(aboutAppInfoWidget, Qt::AlignLeft);

    auto* nameLabel = new QLabel(tr("<b>Kisel %1 by %2</b>").arg(APP_VERSION, APP_AUTHOR), this);
    aboutAppInfoLayout->addWidget(nameLabel);

    auto* licenseLabel = new QLabel(tr("License: GNU GPLv3"), this);
    aboutAppInfoLayout->addWidget(licenseLabel);

    auto* descriptionLabel = new QLabel(tr("<i>Efficient launch of Windows programs</i>"), this);
    aboutAppInfoLayout->addWidget(descriptionLabel);

    auto* settingsBox = new QGroupBox(tr("Settings"), this);
    settingsBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* settingsLayout = new QVBoxLayout(settingsBox);
    settingsLayout->setAlignment(Qt::AlignTop);
    layout->addWidget(settingsBox);

    auto* languageLabel = new QLabel(tr("Language"), this);
    settingsLayout->addWidget(languageLabel);

    auto* languageComboBox = new QComboBox(this);
    languageComboBox->addItems(TRANSLATOR->languageList());
    languageComboBox->setCurrentText(TRANSLATOR->currentLanguageName());
    connect(languageComboBox, &QComboBox::currentTextChanged, this, [](const QString& languageName) {
        TRANSLATOR->saveLanguage(languageName);
    });
    settingsLayout->addWidget(languageComboBox);

    auto* bottomLanguageLine = new QFrame(this);
    bottomLanguageLine->setFrameShape(QFrame::HLine);
    settingsLayout->addWidget(bottomLanguageLine);

    auto* defaultPrefixLabel = new QLabel(tr("Default prefix"), this);
    settingsLayout->addWidget(defaultPrefixLabel);

    auto* individualPrefixCheckBox = new QCheckBox(tr("Individual"), this);
    individualPrefixCheckBox->setChecked(APP_SETTINGS->useIndividualPrefix());
    settingsLayout->addWidget(individualPrefixCheckBox);

    auto* prefixComboBox = new QComboBox(this);
    prefixComboBox->setModel(PREFIX_MODEL);
    prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
    prefixComboBox->setDisabled(APP_SETTINGS->useIndividualPrefix());
    connect(prefixComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultPrefixPath(PREFIX_MODEL->forIndex(index)->path());
    });
    settingsLayout->addWidget(prefixComboBox);

    connect(individualPrefixCheckBox, &QCheckBox::clicked, this, [prefixComboBox](bool checked) {
        APP_SETTINGS->setUseIndividualPrefix(checked);
        prefixComboBox->setDisabled(checked);
    });

    auto* bottomPrefixLine = new QFrame(this);
    bottomPrefixLine->setFrameShape(QFrame::HLine);
    settingsLayout->addWidget(bottomPrefixLine);

    auto* defaultCtLabel = new QLabel(tr("Default compatibility tool"), this);
    settingsLayout->addWidget(defaultCtLabel);

    auto* ctComboBox = new QComboBox(this);
    ctComboBox->setModel(CT_MODEL);
    if (CT_MODEL->defaultCt() != nullptr) {
        ctComboBox->setCurrentIndex(CT_MODEL->ctIndex(CT_MODEL->defaultCt()));
    }
    connect(ctComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultCtPath(CT_MODEL->forIndex(index)->path());
    });
    settingsLayout->addWidget(ctComboBox);

    auto* bottomCtLine = new QFrame(this);
    bottomCtLine->setFrameShape(QFrame::HLine);
    settingsLayout->addWidget(bottomCtLine);

    auto* runtimeAutoUpdateCheckBox = new QCheckBox(tr("Runtime auto-update"), this);
    runtimeAutoUpdateCheckBox->setChecked(APP_SETTINGS->runtimeAutoUpdate());
    connect(runtimeAutoUpdateCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setRuntimeAutoUpdate(checked);
    });
    settingsLayout->addWidget(runtimeAutoUpdateCheckBox);
}
