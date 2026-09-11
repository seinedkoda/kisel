#include "new_ct_dialog.hpp"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"

using namespace kisel;

NewCtDialog::NewCtDialog(QWidget* parent)
    : QDialog(parent)
    , m_ctSourceComboBox(new QComboBox(this))
    , m_releasesComboBox(new QComboBox(this))
    , m_refreshReleasesButton(new QToolButton(this))
    , m_installationLocationsComboBox(new QComboBox(this))
    , m_addToInstallationButton(new QPushButton(QIcon::fromTheme("browser-download"), tr("Add to installation"), this))
{
    setWindowTitle(tr("Install a new tool"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    auto* layout = new QVBoxLayout(this);

    auto* ctSourceLabel = new QLabel(tr("Source:"), this);
    layout->addWidget(ctSourceLabel);

    m_ctSourceComboBox->addItems(CtInstaller::ctSourceMap().keys());
    m_ctSourceComboBox->setPlaceholderText(tr("<No data>"));
    m_ctSourceComboBox->setCurrentText(CtInstaller::defaultCtSource());
    connect(m_ctSourceComboBox, &QComboBox::currentTextChanged, this, &NewCtDialog::fetchAvailableReleases);
    layout->addWidget(m_ctSourceComboBox);

    if (!APP_SETTINGS->deviceSupportsModernVulkan()) {
        auto* infoWidget = new QWidget(this);
        layout->addWidget(infoWidget);

        auto* infoWidgetLayout = new QHBoxLayout(infoWidget);
        infoWidgetLayout->setAlignment(Qt::AlignLeft);

        auto* infoIcon = new QLabel(this);
        infoIcon->setPixmap(QIcon::fromTheme("help-about").pixmap(16, 16));
        infoWidgetLayout->addWidget(infoIcon);

        auto* infoLabel = new QLabel(tr("Your device does not support Vulkan 1.4 or higher, Proton-CachyOS is set by default for compatibility"), this);
        infoLabel->setWordWrap(true);
        infoWidgetLayout->addWidget(infoLabel);
    }

    auto* versionLabel = new QLabel(tr("Version:"), this);
    layout->addWidget(versionLabel);

    auto* releasesWidget = new QWidget(this);
    auto* releasesLayout = new QHBoxLayout(releasesWidget);
    releasesLayout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(releasesWidget);

    m_releasesComboBox->setEnabled(false);
    connect(CT_INSTALLER, &CtInstaller::releasesLoaded, this, &NewCtDialog::onReleasesLoaded);
    releasesLayout->addWidget(m_releasesComboBox);

    m_refreshReleasesButton->setIcon(QIcon::fromTheme("view-refresh"));
    connect(m_refreshReleasesButton, &QToolButton::clicked, this, &NewCtDialog::fetchAvailableReleases);
    releasesLayout->addWidget(m_refreshReleasesButton);

    auto* installationLocationsLabel = new QLabel(tr("Installation location:"), this);
    layout->addWidget(installationLocationsLabel);
    for (const auto& ctDir : CTS_DIR_LIST) {
        if (ctDir.absolutePath().contains(QStringLiteral("steam"))) {
            m_installationLocationsComboBox->addItem(QIcon::fromTheme("steam"), ctDir.path());
        } else {
            m_installationLocationsComboBox->addItem(QIcon(":/icons/kisel.svg"), ctDir.path());
        }
    }
    layout->addWidget(m_installationLocationsComboBox);

    m_addToInstallationButton->setEnabled(false);
    connect(m_addToInstallationButton, &QPushButton::clicked, this, &NewCtDialog::onInstallClicked);
    layout->addWidget(m_addToInstallationButton);

    fetchAvailableReleases();
    adjustSize();
    setFixedSize(size());
}

void NewCtDialog::fetchAvailableReleases()
{
    m_releasesComboBox->clear();
    m_ctSourceComboBox->setEnabled(false);
    m_releasesComboBox->setEnabled(false);
    m_refreshReleasesButton->setEnabled(false);
    m_addToInstallationButton->setEnabled(false);

    CT_INSTALLER->fetchReleases(m_ctSourceComboBox->currentText(), this);
}

void NewCtDialog::onReleasesLoaded(QObject* requester, const QMap<QString, QUrl>& releaseMap, bool success, const QString& errorText)
{
    if (requester != this) {
        return;
    }

    auto i = releaseMap.cend();
    while (i != releaseMap.cbegin()) {
        --i;
        m_releasesComboBox->addItem(i.key(), i.value());
    }

    bool listIsEmpty = releaseMap.isEmpty();
    m_ctSourceComboBox->setEnabled(true);
    m_releasesComboBox->setDisabled(listIsEmpty);
    m_refreshReleasesButton->setEnabled(true);
    m_addToInstallationButton->setDisabled(listIsEmpty);

    if (!success) {
        QMessageBox::critical(this, tr("Update error"), tr("Error loading releases: %1").arg(errorText));
    }
}

void NewCtDialog::onInstallClicked()
{
    CT_INSTALLER->addToInstallation(m_releasesComboBox->currentText(), m_releasesComboBox->currentData().toUrl(), m_installationLocationsComboBox->currentText());
    close();
}
