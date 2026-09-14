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
    , m_oldDeviceInfoWidget(new OldDeviceInfoWidget(this))
{
    setWindowTitle(tr("Install a new tool"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* ctSourceLabel = new QLabel(tr("Source:"), this);
    layout->addWidget(ctSourceLabel);

    m_ctSourceComboBox->addItems(CtInstaller::ctSourceMap().keys());
    m_ctSourceComboBox->setPlaceholderText(tr("<No data>"));
    m_ctSourceComboBox->setCurrentText(CtInstaller::defaultCtSource());
    connect(m_ctSourceComboBox, &QComboBox::currentTextChanged, this, &NewCtDialog::fetchAvailableReleases);
    layout->addWidget(m_ctSourceComboBox);

    connect(m_ctSourceComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(m_ctSourceComboBox->currentText()));
    });

    layout->addWidget(m_oldDeviceInfoWidget);
    m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(m_ctSourceComboBox->currentText()));

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

    layout->addStretch();

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    buttonBox->addButton(m_addToInstallationButton, QDialogButtonBox::AcceptRole);
    m_addToInstallationButton->setEnabled(false);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &NewCtDialog::onInstallClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttonBox);

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
    accept();
}
