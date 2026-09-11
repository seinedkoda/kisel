#include "ct_list_widget.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/compatibilitytools/ct_installer.hpp"
#include "ui/compatibilitytools/new_ct_dialog.hpp"

using namespace kisel;

CtListWidget::CtListWidget(QWidget* parent)
    : QWidget(parent)
    , m_ctTableView(new QTableView(this))
{
    setWindowTitle(tr("Kisel — Compatibility Tools"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* layout = new QVBoxLayout(this);

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
    connect(m_ctTableView, &QTableView::customContextMenuRequested, this, &CtListWidget::onContextMenuRequested);
    layout->addWidget(m_ctTableView);

    auto* addNewCtDialogButton = new QPushButton(QIcon::fromTheme("list-add"), tr("Install a new one"), this);
    connect(addNewCtDialogButton, &QPushButton::clicked, this, &CtListWidget::openAddNewCtDialog);
    layout->addWidget(addNewCtDialogButton);

    connect(CT_INSTALLER, &CtInstaller::installationError, this, &CtListWidget::onInstallationError);
}

void CtListWidget::openAddNewCtDialog()
{
    auto* addNewCtDialog = new NewCtDialog(this);
    addNewCtDialog->exec();
}

void CtListWidget::onInstallationError(const QString& errorText)
{
    QMessageBox::critical(this, tr("Installation error"), errorText);
}

void CtListWidget::onContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = m_ctTableView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    Ct* ct = CT_MODEL->forIndex(index.row());

    QMenu menu(this);

    const Ct::Status status = ct->status();
    if (status == Ct::Installed) {
        QAction* openAction = menu.addAction(QIcon::fromTheme("document-open-folder"), tr("Open in files"));
        connect(openAction, &QAction::triggered, this, [ct]() { QDesktopServices::openUrl(QUrl::fromLocalFile(ct->path())); });

        QAction* deleteAction = menu.addAction(QIcon::fromTheme("entry-delete"), tr("Delete"));
        connect(deleteAction, &QAction::triggered, this, [this, ct, index]() {
            if (QMessageBox::question(this, tr("Confirmation required"), tr("Delete \"%1\"?").arg(ct->name())) == QMessageBox::Yes) {
                CT_MODEL->removeRow(index.row());
            }
        });
    } else if (status == Ct::Downloading || status == Ct::Unpacking) {
        QAction* stopAction = menu.addAction(QIcon::fromTheme("media-playback-stop"), tr("Cancel"));
        connect(stopAction, &QAction::triggered, this, [this, ct]() {
            auto answer = QMessageBox::question(this, tr("Confirmation required"), tr("Cancel the installation process of \"%1\"?").arg(ct->name()));
            if (answer == QMessageBox::Yes) {
                CT_INSTALLER->cancelInstallation(ct);
            }
        });
    }

    menu.exec(QCursor::pos());
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
