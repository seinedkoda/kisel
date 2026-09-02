#include "ct_window.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

#include "app_settings.hpp"
#include "ct_installer.hpp"
#include "ct_model.hpp"

using namespace kisel;

CtWindow::CtWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ctTableView(new QTableView(this))
{
    setWindowTitle(tr("Kisel — Compatibility Tools"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    resize(400, height());

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QVBoxLayout(centralWidget);

    auto* listLabel = new QLabel(tr("<h3>Compatibility Tools</h3>"), this);
    layout->addWidget(listLabel);

    m_ctTableView->setModel(CT_MODEL);
    auto* delegate = new ProgressBarDelegate(m_ctTableView);
    m_ctTableView->setItemDelegateForColumn(1, delegate);
    m_ctTableView->resizeColumnsToContents();
    m_ctTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_ctTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_ctTableView->verticalHeader()->hide();
    m_ctTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ctTableView, &QTableView::customContextMenuRequested, this, &CtWindow::onContextMenuRequested);
    layout->addWidget(m_ctTableView);

    auto* addNewCtDialogButton = new QPushButton(QIcon::fromTheme("list-add"), tr("Install a new one"), this);
    connect(addNewCtDialogButton, &QPushButton::clicked, this, &CtWindow::openAddNewCtDialog);
    layout->addWidget(addNewCtDialogButton);

    connect(CT_INSTALLER, &CtInstaller::installationError, this, &CtWindow::onInstallationError);
}

void CtWindow::openAddNewCtDialog()
{
    auto* addNewCtDialog = new AddNewCtDialog(this);
    addNewCtDialog->exec();
}

void CtWindow::onInstallationError(const QString& errorText)
{
    QMessageBox::critical(this, tr("Installation error"), errorText);
}

void CtWindow::onContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = m_ctTableView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    Ct* ct = CT_MODEL->forIndex(index.row());

    auto* menu = new QMenu(this);

    const Ct::Status status = ct->status();
    if (status == Ct::Installed) {
        QAction* openAction = menu->addAction(QIcon::fromTheme("document-open-folder"), tr("Open in files"));
        connect(openAction, &QAction::triggered, this, [ct]() { QDesktopServices::openUrl(QUrl::fromLocalFile(ct->path())); });

        QAction* deleteAction = menu->addAction(QIcon::fromTheme("entry-delete"), tr("Delete"));
        connect(deleteAction, &QAction::triggered, this, [this, ct, index]() {
            if (QMessageBox::question(this, tr("Confirmation required"), tr("Delete \"%1\"?").arg(ct->name())) == QMessageBox::Yes) {
                CT_MODEL->removeRow(index.row());
            }
        });
    } else if (status == Ct::Downloading || status == Ct::Unpacking) {
        QAction* stopAction = menu->addAction(QIcon::fromTheme("media-playback-stop"), tr("Cancel"));
        connect(stopAction, &QAction::triggered, this, [this, ct]() {
            auto answer = QMessageBox::question(this, tr("Confirmation required"), tr("Cancel the installation process of \"%1\"?").arg(ct->name()));
            if (answer == QMessageBox::Yes) {
                CT_INSTALLER->cancelInstallation(ct);
            }
        });
    }

    menu->exec(QCursor::pos());
}

void ProgressBarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 1) {
        auto status = index.data(CtModel::StatusRole).value<Ct::Status>();
        if (status == Ct::Downloading) {
            int progress = index.data(CtModel::ProgressRole).toInt();

            QStyleOptionProgressBar progressBarOption;
            progressBarOption.rect = option.rect.adjusted(4, 4, -4, -4);
            progressBarOption.minimum = 0;
            progressBarOption.maximum = 100;
            progressBarOption.progress = progress;
            progressBarOption.text = QString::number(progress) % "%";
            progressBarOption.textVisible = true;
            progressBarOption.textAlignment = Qt::AlignCenter;

            QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
            return;
        }
    }
    QStyledItemDelegate::paint(painter, option, index);
}

AddNewCtDialog::AddNewCtDialog(QWidget* parent)
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
    connect(m_ctSourceComboBox, &QComboBox::currentTextChanged, this, &AddNewCtDialog::fetchAvailableReleases);
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
    connect(CT_INSTALLER, &CtInstaller::releasesLoaded, this, &AddNewCtDialog::onReleasesLoaded);
    releasesLayout->addWidget(m_releasesComboBox);

    m_refreshReleasesButton->setIcon(QIcon::fromTheme("view-refresh"));
    connect(m_refreshReleasesButton, &QToolButton::clicked, this, &AddNewCtDialog::fetchAvailableReleases);
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
    connect(m_addToInstallationButton, &QPushButton::clicked, this, &AddNewCtDialog::onInstallClicked);
    layout->addWidget(m_addToInstallationButton);

    fetchAvailableReleases();
    adjustSize();
    setFixedSize(size());
}

void AddNewCtDialog::fetchAvailableReleases()
{
    m_releasesComboBox->clear();
    m_ctSourceComboBox->setEnabled(false);
    m_releasesComboBox->setEnabled(false);
    m_refreshReleasesButton->setEnabled(false);
    m_addToInstallationButton->setEnabled(false);

    CT_INSTALLER->fetchReleases(m_ctSourceComboBox->currentText(), this);
}

void AddNewCtDialog::onReleasesLoaded(QObject* requester, const QMap<QString, QUrl>& releaseMap, bool success, const QString& errorText)
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

void AddNewCtDialog::onInstallClicked()
{
    CT_INSTALLER->addToInstallation(m_releasesComboBox->currentText(), m_releasesComboBox->currentData().toUrl(), m_installationLocationsComboBox->currentText());
    close();
}
