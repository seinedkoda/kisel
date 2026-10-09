#include "app_prefixes_settings_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/settings/app_settings.hpp"
#include "ui/prefixes/prefix_list_widget.hpp"

using namespace kisel;

AppPrefixesSettingsPage::AppPrefixesSettingsPage(AppSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
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
    prefixTypeComboBox->setCurrentIndex(m_settings->prefixType());
    layout->addWidget(prefixTypeComboBox);

    connect(prefixTypeComboBox, &QComboBox::activated, this, [this](int type) {
        m_settings->setPrefixType(static_cast<AppSettings::PrefixType>(type));
    });

    auto* sharedPrefixesDirLabel = new QLabel(tr("Directory for shared prefixes"), this);
    layout->addWidget(sharedPrefixesDirLabel);

    auto* sharedPrefixesDirWidget = new QWidget(this);
    layout->addWidget(sharedPrefixesDirWidget);

    auto* sharedPrefixesDirLayout = new QHBoxLayout(sharedPrefixesDirWidget);
    sharedPrefixesDirLayout->setContentsMargins(0, 0, 0, 0);

    m_sharedPrefixesDirLineEdit->setText(m_settings->prefixesDir().path());
    m_sharedPrefixesDirLineEdit->setDisabled(true);
    sharedPrefixesDirLayout->addWidget(m_sharedPrefixesDirLineEdit);

    m_sharedPrefixesDirSelectButton->setIcon(QIcon::fromTheme("document-open"));
    m_sharedPrefixesDirSelectButton->setToolTip(tr("Select a new path for prefixes"));
    connect(m_sharedPrefixesDirSelectButton, &QToolButton::clicked, this, &AppPrefixesSettingsPage::onPrefixesDirSelectClicked);
    sharedPrefixesDirLayout->addWidget(m_sharedPrefixesDirSelectButton);

    m_sharedPrefixesDirResetButton->setIcon(QIcon::fromTheme("document-revert"));
    m_sharedPrefixesDirResetButton->setToolTip(tr("Restore the original path to prefixes"));
    m_sharedPrefixesDirResetButton->setDisabled(m_settings->prefixesDir() == m_settings->appPrefixesDir());
    connect(m_sharedPrefixesDirResetButton, &QToolButton::clicked, this, &AppPrefixesSettingsPage::onPrefixesDirResetClicked);
    sharedPrefixesDirLayout->addWidget(m_sharedPrefixesDirResetButton);

    auto* defaultPrefixLabel = new QLabel(tr("Default shared prefix"), this);
    layout->addWidget(defaultPrefixLabel);

    auto* sharedPrefixComboBox = new QComboBox(this);
    sharedPrefixComboBox->setModel(PREFIX_MODEL);
    sharedPrefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
    connect(sharedPrefixComboBox, &QComboBox::currentTextChanged, this, [this](const QString& name) {
        m_settings->setDefaultPrefixName(name);
    });
    layout->addWidget(sharedPrefixComboBox);

    auto* bottomDefaultPrefixLine = new QFrame(this);
    bottomDefaultPrefixLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomDefaultPrefixLine);

    auto* prefixListWidget = new PrefixListWidget(this);
    prefixListWidget->layout()->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(prefixListWidget);
}

void AppPrefixesSettingsPage::onPrefixesDirSelectClicked()
{
    const QString dirPath = QFileDialog::getExistingDirectory(
        this,
        tr("Select directory"),
        m_settings->prefixesDir().path(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dirPath.isEmpty()) {
        m_settings->setPrefixesDir(dirPath);
        m_sharedPrefixesDirLineEdit->setText(dirPath);
        m_sharedPrefixesDirResetButton->setDisabled(dirPath == m_settings->appPrefixesDir());
        PREFIX_MODEL->refreshList();
    }
}

void AppPrefixesSettingsPage::onPrefixesDirResetClicked()
{
    m_settings->remove("prefixesDir");
    m_sharedPrefixesDirLineEdit->setText(m_settings->prefixesDir().path());
    m_sharedPrefixesDirResetButton->setDisabled(true);
    PREFIX_MODEL->refreshList();
}