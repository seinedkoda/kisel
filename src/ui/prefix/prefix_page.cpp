#include "prefix_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"
#include "ui/prefix/prefix_list_widget.hpp"

using namespace kisel;

PrefixPage::PrefixPage(QWidget* parent)
    : QWidget(parent)
    , m_sharedPrefixesDirLineEdit(new QLineEdit(this))
    , m_sharedPrefixesDirSelectButton(new QToolButton(this))
    , m_sharedPrefixesDirResetButton(new QToolButton(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* defaultPrefixTypeLabel = new QLabel(tr("Default prefix type"), this);
    layout->addWidget(defaultPrefixTypeLabel);

    auto* prefixTypeComboBox = new QComboBox(this);
    prefixTypeComboBox->addItems({ tr("Shared"), tr("Individual"), tr("Portable") });
    prefixTypeComboBox->setCurrentIndex(APP_SETTINGS->prefixType());
    layout->addWidget(prefixTypeComboBox);

    connect(prefixTypeComboBox, &QComboBox::activated, this, [](int type) {
        APP_SETTINGS->setPrefixType(static_cast<AppSettings::PrefixType>(type));
    });

    auto* sharedPrefixesDirLabel = new QLabel(tr("Directory for shared prefixes"), this);
    layout->addWidget(sharedPrefixesDirLabel);

    auto* sharedPrefixesDirWidget = new QWidget(this);
    layout->addWidget(sharedPrefixesDirWidget);

    auto* sharedPrefixesDirLayout = new QHBoxLayout(sharedPrefixesDirWidget);
    sharedPrefixesDirLayout->setContentsMargins(0, 0, 0, 0);

    m_sharedPrefixesDirLineEdit->setText(APP_SETTINGS->prefixesDir().path());
    m_sharedPrefixesDirLineEdit->setDisabled(true);
    sharedPrefixesDirLayout->addWidget(m_sharedPrefixesDirLineEdit);

    m_sharedPrefixesDirSelectButton->setIcon(QIcon::fromTheme("document-open"));
    m_sharedPrefixesDirSelectButton->setToolTip(tr("Select a new path for prefixes"));
    connect(m_sharedPrefixesDirSelectButton, &QToolButton::clicked, this, &PrefixPage::onPrefixesDirSelectClicked);
    sharedPrefixesDirLayout->addWidget(m_sharedPrefixesDirSelectButton);

    m_sharedPrefixesDirResetButton->setIcon(QIcon::fromTheme("document-revert"));
    m_sharedPrefixesDirResetButton->setToolTip(tr("Restore the original path to prefixes"));
    m_sharedPrefixesDirResetButton->setDisabled(APP_SETTINGS->prefixesDir() == APP_SETTINGS->appPrefixesDir());
    connect(m_sharedPrefixesDirResetButton, &QToolButton::clicked, this, &PrefixPage::onPrefixesDirResetClicked);
    sharedPrefixesDirLayout->addWidget(m_sharedPrefixesDirResetButton);

    auto* defaultPrefixLabel = new QLabel(tr("Default shared prefix"), this);
    layout->addWidget(defaultPrefixLabel);

    auto* sharedPrefixComboBox = new QComboBox(this);
    sharedPrefixComboBox->setModel(PREFIX_MODEL);
    sharedPrefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
    connect(sharedPrefixComboBox, &QComboBox::currentTextChanged, this, [](const QString& name) {
        APP_SETTINGS->setDefaultPrefixName(name);
    });
    layout->addWidget(sharedPrefixComboBox);

    auto* bottomDefaultPrefixLine = new QFrame(this);
    bottomDefaultPrefixLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomDefaultPrefixLine);

    auto* prefixListWidget = new PrefixListWidget(this);
    prefixListWidget->layout()->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(prefixListWidget);
}

void PrefixPage::onPrefixesDirSelectClicked()
{
    const QString dirPath = QFileDialog::getExistingDirectory(
        this,
        tr("Select directory"),
        APP_SETTINGS->prefixesDir().path(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dirPath.isEmpty()) {
        APP_SETTINGS->setPrefixesDir(dirPath);
        m_sharedPrefixesDirLineEdit->setText(dirPath);
        m_sharedPrefixesDirResetButton->setDisabled(dirPath == APP_SETTINGS->appPrefixesDir());
        PREFIX_MODEL->refreshList();
    }
}

void PrefixPage::onPrefixesDirResetClicked()
{
    APP_SETTINGS->remove("prefixesDir");
    m_sharedPrefixesDirLineEdit->setText(APP_SETTINGS->prefixesDir().path());
    m_sharedPrefixesDirResetButton->setDisabled(true);
    PREFIX_MODEL->refreshList();
}