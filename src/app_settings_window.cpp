#include "app_settings_window.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyleFactory>

#include "app_settings.hpp"
#include "ct_model.hpp"
#include "prefix_model.hpp"
#include "translator.hpp"

using namespace kisel;

AppSettingsWindow::AppSettingsWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Kisel — Settings"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumWidth(400);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QVBoxLayout(centralWidget);

    auto* titleLabel = new QLabel(tr("<h3>Global settings</h3>"));
    layout->addWidget(titleLabel);

    auto* helpLabel = new QLabel(tr("<i>For detailed settings, go to the prefix context menu in the main window</i>"), this);
    helpLabel->setWordWrap(true);
    layout->addWidget(helpLabel);

    auto* tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget);

    auto* generalTab = new QWidget(this);
    tabWidget->addTab(generalTab, tr("General"));

    auto* generalTabLayout = new QVBoxLayout(generalTab);
    generalTabLayout->setAlignment(Qt::AlignTop);

    auto* languageLabel = new QLabel(tr("Language"), this);
    generalTabLayout->addWidget(languageLabel);

    auto* languageComboBox = new QComboBox(this);
    languageComboBox->addItems(TRANSLATOR->languageList());
    languageComboBox->setCurrentText(TRANSLATOR->currentLanguageName());
    connect(languageComboBox, &QComboBox::currentTextChanged, this, [](const QString& languageName) {
        TRANSLATOR->saveLanguage(languageName);
    });
    generalTabLayout->addWidget(languageComboBox);

    auto* styleLabel = new QLabel(tr("Style"), this);
    generalTabLayout->addWidget(styleLabel);

    auto* styleComboBox = new QComboBox(this);
    static QStringList styles = QStyleFactory::keys();
    styleComboBox->addItems(styles);
    const QString savedStyle = APP_SETTINGS->styleName();
    if (styles.contains(savedStyle)) {
        styleComboBox->setCurrentText(savedStyle);
    } else {
        styleComboBox->setCurrentText(QStringLiteral("Fusion"));
    }
    connect(styleComboBox, &QComboBox::currentTextChanged, this, [](const QString& styleName) { APP_SETTINGS->setStyleName(styleName); });
    generalTabLayout->addWidget(styleComboBox);

    auto* loggingCheckBox = new QCheckBox(tr("Logging"), this);
    loggingCheckBox->setChecked(APP_SETTINGS->loggingEnabled());
    connect(loggingCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setLoggingEnabled(checked);
    });
    generalTabLayout->addWidget(loggingCheckBox);

    auto* bottomLanguageLine = new QFrame(this);
    bottomLanguageLine->setFrameShape(QFrame::HLine);
    generalTabLayout->addWidget(bottomLanguageLine);

    auto* defaultPrefixLabel = new QLabel(tr("Default prefix"), this);
    generalTabLayout->addWidget(defaultPrefixLabel);

    auto* individualPrefixCheckBox = new QCheckBox(tr("Individual"), this);
    individualPrefixCheckBox->setChecked(APP_SETTINGS->useIndividualPrefix());
    generalTabLayout->addWidget(individualPrefixCheckBox);

    auto* prefixComboBox = new QComboBox(this);
    prefixComboBox->setModel(PREFIX_MODEL);
    prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
    prefixComboBox->setDisabled(APP_SETTINGS->useIndividualPrefix());
    connect(prefixComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultPrefixPath(PREFIX_MODEL->forIndex(index)->path());
    });
    generalTabLayout->addWidget(prefixComboBox);

    connect(individualPrefixCheckBox, &QCheckBox::clicked, this, [prefixComboBox](bool checked) {
        APP_SETTINGS->setUseIndividualPrefix(checked);
        prefixComboBox->setDisabled(checked);
    });

    auto* bottomPrefixLine = new QFrame(this);
    bottomPrefixLine->setFrameShape(QFrame::HLine);
    generalTabLayout->addWidget(bottomPrefixLine);

    auto* defaultCtLabel = new QLabel(tr("Default compatibility tool"), this);
    generalTabLayout->addWidget(defaultCtLabel);

    auto* ctComboBox = new QComboBox(this);
    ctComboBox->setPlaceholderText(tr("<No installed>"));
    auto* ctInstalledProxyModel = new CtInstalledProxyModel(this);
    ctInstalledProxyModel->setSourceModel(CT_MODEL);
    ctComboBox->setModel(ctInstalledProxyModel);
    if (CT_MODEL->defaultCt() != nullptr) {
        ctComboBox->setCurrentIndex(CT_MODEL->ctIndex(CT_MODEL->defaultCt()));
    }
    connect(ctComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultCtPath(CT_MODEL->forIndex(index)->path());
    });
    generalTabLayout->addWidget(ctComboBox);

    auto* umuTab = new QWidget(this);
    tabWidget->addTab(umuTab, "UMU");

    auto* umuTabLayout = new QVBoxLayout(umuTab);
    umuTabLayout->setAlignment(Qt::AlignTop);

    auto* umuPathComboBox = new QComboBox(this);
    if (APP_SETTINGS->isFlatpak()) {
        umuPathComboBox->addItem(tr("Built-in (Flatpak)"), false);
        umuPathComboBox->setDisabled(true);
    } else {
        umuPathComboBox->addItem(tr("Built-in"), false);
        umuPathComboBox->addItem(tr("System"), true);
        umuPathComboBox->setCurrentIndex(APP_SETTINGS->useSystemUMU() ? 1 : 0);
        connect(umuPathComboBox, &QComboBox::activated, this, [umuPathComboBox]() {
            APP_SETTINGS->setUseSystemUMU(umuPathComboBox->currentData().toBool());
        });
    }
    umuTabLayout->addWidget(umuPathComboBox);

    auto* runtimeAutoUpdateCheckBox = new QCheckBox(tr("Runtime auto-update"), this);
    runtimeAutoUpdateCheckBox->setChecked(APP_SETTINGS->runtimeAutoUpdate());
    connect(runtimeAutoUpdateCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setRuntimeAutoUpdate(checked);
    });
    umuTabLayout->addWidget(runtimeAutoUpdateCheckBox);
}
