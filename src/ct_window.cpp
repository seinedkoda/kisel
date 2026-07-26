#include "ct_window.hpp"

#include <QComboBox>
#include <QDesktopServices>
#include <QGroupBox>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QToolButton>
#include <QCloseEvent>
#include <QVBoxLayout>

#include "app_settings.hpp"
#include "ct_model.hpp"

using namespace kisel;

CtWindow::CtWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ctListView(new QListView(this))
    , m_ctSourceComboBox(new QComboBox(this))
    , m_releasesComboBox(new QComboBox(this))
    , m_refreshReleasesButton(new QToolButton(this))
    , m_installationLocationsComboBox(new QComboBox(this))
    , m_progressBar(new QProgressBar(this))
    , m_listViewContextMenu(new QMenu(this))
    , m_installCancelButton(new QPushButton(QIcon::fromTheme("browser-download"), tr("Install"), this))
{
    setWindowTitle(tr("Kisel — Compatibility Tools"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QVBoxLayout(centralWidget);

    auto* listLabel = new QLabel(tr("Installed compatibility tools:"), this);
    layout->addWidget(listLabel);

    m_ctListView->setModel(CT_MODEL);
    m_ctListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ctListView, &QListView::customContextMenuRequested, this, &CtWindow::onContextMenuRequested);
    layout->addWidget(m_ctListView);

    m_openAction = m_listViewContextMenu->addAction(QIcon::fromTheme("document-open-folder"), tr("Open in files"));
    m_deleteAction = m_listViewContextMenu->addAction(QIcon::fromTheme("remove"), tr("Delete"));

    auto* installationGroupBox = new QGroupBox(tr("Install a new tool"), this);
    auto* installationBoxLayout = new QVBoxLayout(installationGroupBox);
    layout->addWidget(installationGroupBox);

    auto* ctSourceLabel = new QLabel(tr("Source:"), this);
    installationBoxLayout->addWidget(ctSourceLabel);

    m_ctSourceComboBox->addItems(CT_MODEL->ctSourceMap().keys());
    m_ctSourceComboBox->setCurrentText(CT_MODEL->ctSourceName());
    installationBoxLayout->addWidget(m_ctSourceComboBox);

    auto* versionLabel = new QLabel(tr("Version:"), this);
    installationBoxLayout->addWidget(versionLabel);

    auto* releasesWidget = new QWidget(this);
    auto* releasesLayout = new QHBoxLayout(releasesWidget);
    releasesLayout->setContentsMargins(0, 0, 0, 0);
    installationBoxLayout->addWidget(releasesWidget);

    m_releasesComboBox->setEnabled(false);
    releasesLayout->addWidget(m_releasesComboBox);

    m_refreshReleasesButton->setIcon(QIcon::fromTheme("view-refresh"));
    releasesLayout->addWidget(m_refreshReleasesButton);

    auto* installationLocationsLabel = new QLabel(tr("Installation location:"), this);
    installationBoxLayout->addWidget(installationLocationsLabel);
    for (const auto& ctDir : CTS_DIR_LIST) {
        if (ctDir.absolutePath().contains("steam")) {
            m_installationLocationsComboBox->addItem(QIcon::fromTheme("steam"), ctDir.path());
        } else {
            m_installationLocationsComboBox->addItem(QIcon(":/icons/kisel.svg"), ctDir.path());
        }
    }
    installationBoxLayout->addWidget(m_installationLocationsComboBox);

    m_installCancelButton->setEnabled(false);
    installationBoxLayout->addWidget(m_installCancelButton);

    m_progressBar->setValue(0);
    m_progressBar->setFormat("%p%");
    statusBar()->addPermanentWidget(m_progressBar);
    statusBar()->hide();

    connect(CT_MODEL, &CtModel::fetchReleasesStarted, this, &CtWindow::onFetchReleasesStarted);
    connect(CT_MODEL, &CtModel::fetchReleasesFinished, this, &CtWindow::onFetchReleasesFinished);
    connect(CT_MODEL, &CtModel::fetchReleasesError, this, &CtWindow::onFetchReleasesError);
    connect(CT_MODEL, &CtModel::downloadStarted, this, &CtWindow::onDownloadStarted);
    connect(CT_MODEL, &CtModel::downloadProgressChanged, this, &CtWindow::onDownloadProgressChanged);
    connect(CT_MODEL, &CtModel::extractStarted, this, &CtWindow::onExtractionStarted);
    connect(CT_MODEL, &CtModel::extractFinished, this, &CtWindow::onExtractionFinished);
    connect(CT_MODEL, &CtModel::installationError, this, &CtWindow::onInstallationError);

    connect(m_ctSourceComboBox, &QComboBox::currentTextChanged, this, [](const QString& text) {
        CT_MODEL->setCtSourceFromName(text);
        CT_MODEL->fetchAvailableReleases();
    });

    connect(m_refreshReleasesButton, &QToolButton::clicked, this, [this]() {
        CT_MODEL->fetchAvailableReleases();
    });

    connect(m_installCancelButton, &QPushButton::clicked, this, [this]() {
        if (CT_MODEL->installationIsRunning()) {
            auto answer = QMessageBox::question(this, tr("Confirmation"), tr("Cancel the installation process?"));
            if (answer == QMessageBox::Yes) {
                CT_MODEL->cancelInstallation();
            }
        } else {
            CT_MODEL->installRelease(m_releasesComboBox->currentText(), m_installationLocationsComboBox->currentText());
        }
    });

    CT_MODEL->setCtSourceFromName(m_ctSourceComboBox->currentText());
    CT_MODEL->fetchAvailableReleases();
}

void CtWindow::onFetchReleasesStarted()
{
    m_releasesComboBox->clear();
    m_ctSourceComboBox->setEnabled(false);
    m_releasesComboBox->setEnabled(false);
    m_refreshReleasesButton->setEnabled(false);
    m_installCancelButton->setEnabled(false);
}

void CtWindow::onFetchReleasesFinished()
{
    QStringList releasesList = CT_MODEL->availableReleasesList();
    m_releasesComboBox->addItems(releasesList);

    bool listIsEmpty = releasesList.isEmpty();
    m_ctSourceComboBox->setEnabled(true);
    m_releasesComboBox->setDisabled(listIsEmpty);
    m_refreshReleasesButton->setEnabled(true);
    m_installCancelButton->setDisabled(listIsEmpty);
}

void CtWindow::onFetchReleasesError(const QString& errorText)
{
    m_releasesComboBox->clear();
    m_ctSourceComboBox->setEnabled(true);
    m_refreshReleasesButton->setEnabled(true);
    QMessageBox::critical(this, tr("Error loading releases"), errorText);
}

void CtWindow::onDownloadStarted()
{
    m_ctSourceComboBox->setEnabled(false);
    m_releasesComboBox->setEnabled(false);
    m_refreshReleasesButton->setEnabled(false);
    m_installationLocationsComboBox->setEnabled(false);
    m_installCancelButton->setText(tr("Cancel"));
    m_installCancelButton->setIcon(QIcon::fromTheme("stop"));
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    statusBar()->show();
    statusBar()->showMessage(tr("Downloading"));
}

void CtWindow::onDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        m_progressBar->setValue(percentage);
    }
}

void CtWindow::onExtractionStarted()
{
    m_progressBar->setRange(0, 0);
    statusBar()->showMessage(tr("Extracting"));
}

void CtWindow::resetInstallationWidgetsState()
{
    m_ctSourceComboBox->setEnabled(true);
    m_releasesComboBox->setEnabled(true);
    m_refreshReleasesButton->setEnabled(true);
    m_installationLocationsComboBox->setEnabled(true);
    m_installCancelButton->setText("Install");
    m_installCancelButton->setIcon(QIcon::fromTheme("browser-download"));
    statusBar()->hide();
}

void CtWindow::onExtractionFinished()
{
    resetInstallationWidgetsState();
    QMessageBox::information(this, tr("Completed"), tr("Successfully installed!"));
}

void CtWindow::onInstallationError(const QString& errorText)
{
    resetInstallationWidgetsState();
    QMessageBox::critical(this, tr("Install error"), errorText);
}

void CtWindow::onInstallationCanceled()
{
    resetInstallationWidgetsState();
}

void CtWindow::closeEvent(QCloseEvent* event)
{
    if (CT_MODEL->installationIsRunning()) {
        auto answer = QMessageBox::question(this, tr("Confirmation"), tr("Cancel the installation process and close the window?"));
        if (answer == QMessageBox::Yes) {
            CT_MODEL->cancelInstallation();
        } else {
            event->ignore();
            return;
        }
    }

    event->accept();
    QMainWindow::closeEvent(event);
}

void CtWindow::onContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = m_ctListView->indexAt(pos);

    if (!index.isValid()) {
        return;
    }

    QAction* selectedAction = m_listViewContextMenu->exec(pos);

    if (selectedAction == m_openAction) {
        QString prefixPath = CT_MODEL->data(index, CtModel::PathRole).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(prefixPath));
    } else if (selectedAction == m_deleteAction) {
        QString ctName = index.data().toString();
        if (QMessageBox::question(this, tr("Confirm"), tr("Remove \"%1\"?").arg(ctName)) == QMessageBox::Yes) {
            CT_MODEL->remove(index);
        }
    }
}
