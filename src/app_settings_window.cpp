#include "app_settings_window.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <qcombobox.h>
#include <qlabel.h>
#include <qnamespace.h>

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
    layout->setAlignment(Qt::AlignTop);
    setCentralWidget(centralWidget);

    auto* languageLabel = new QLabel(tr("Language"), this);
    layout->addWidget(languageLabel);

    auto* languageComboBox = new QComboBox(this);
    languageComboBox->addItems(TRANSLATOR->languageList());
    languageComboBox->setCurrentText(TRANSLATOR->currentLanguageName());
    connect(languageComboBox, &QComboBox::currentTextChanged, this, [](const QString& languageName) {
        TRANSLATOR->saveLanguage(languageName);
    });
    layout->addWidget(languageComboBox);

    auto* bottomLanguageLine = new QFrame(this);
    bottomLanguageLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomLanguageLine);

    auto* defaultPrefixLabel = new QLabel(tr("Default prefix"), this);
    layout->addWidget(defaultPrefixLabel);

    auto* individualPrefixCheckBox = new QCheckBox(tr("Individual"), this);
    individualPrefixCheckBox->setChecked(APP_SETTINGS->useIndividualPrefix());
    layout->addWidget(individualPrefixCheckBox);

    auto* prefixComboBox = new QComboBox(this);
    prefixComboBox->setModel(PREFIX_MODEL);
    prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
    prefixComboBox->setDisabled(APP_SETTINGS->useIndividualPrefix());
    connect(prefixComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultPrefixPath(PREFIX_MODEL->forIndex(index)->path());
    });
    layout->addWidget(prefixComboBox);

    connect(individualPrefixCheckBox, &QCheckBox::clicked, this, [prefixComboBox](bool checked) {
        APP_SETTINGS->setUseIndividualPrefix(checked);
        prefixComboBox->setDisabled(checked);
    });

    auto* bottomPrefixLine = new QFrame(this);
    bottomPrefixLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomPrefixLine);

    auto* defaultCtLabel = new QLabel(tr("Default compatibility tool"), this);
    layout->addWidget(defaultCtLabel);

    auto* ctComboBox = new QComboBox(this);
    ctComboBox->setModel(CT_MODEL);
    if (CT_MODEL->defaultCt() != nullptr) {
        ctComboBox->setCurrentIndex(CT_MODEL->ctIndex(CT_MODEL->defaultCt()));
    }
    connect(ctComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultCtPath(CT_MODEL->forIndex(index)->path());
    });
    layout->addWidget(ctComboBox);

    auto* bottomCtLine = new QFrame(this);
    bottomCtLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomCtLine);

    auto* umuLabel = new QLabel("UMU", this);
    layout->addWidget(umuLabel);

    auto* umuPathComboBox = new QComboBox(this);
    umuPathComboBox->addItem(tr("Built-in"), false);
    umuPathComboBox->addItem(tr("System"), true);
    umuPathComboBox->setCurrentIndex(APP_SETTINGS->useSystemUMU() ? 1 : 0);
    connect(umuPathComboBox, &QComboBox::activated, this, [umuPathComboBox]() {
        APP_SETTINGS->setUseSystemUMU(umuPathComboBox->currentData().toBool());
    });
    umuPathComboBox->setDisabled(APP_SETTINGS->isFlatpak());
    layout->addWidget(umuPathComboBox);

    auto* runtimeAutoUpdateCheckBox = new QCheckBox(tr("Runtime auto-update"), this);
    runtimeAutoUpdateCheckBox->setChecked(APP_SETTINGS->runtimeAutoUpdate());
    connect(runtimeAutoUpdateCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setRuntimeAutoUpdate(checked);
    });
    layout->addWidget(runtimeAutoUpdateCheckBox);
}
