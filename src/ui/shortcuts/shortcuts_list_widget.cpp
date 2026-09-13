#include "shortcuts_list_widget.hpp"

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <ranges>

#include "core/app/app.hpp"
#include "core/shortcuts/shortcut_model.hpp"
#include "ui/shortcuts/edit_shortcuts_dialog.hpp"

using namespace kisel;

ShortcutsListWidget::ShortcutsListWidget(QWidget* parent)
    : QWidget(parent)
    , m_proxyModel(new ShortcutProxyModel(this))
    , m_tableView(new QTableView(this))
{
    setWindowTitle(tr("Kisel — Shortcuts"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* layout = new QVBoxLayout(this);

    auto* listLabel = new QLabel(tr("<h3>Shortcuts</h3>"), this);
    layout->addWidget(listLabel);

    m_proxyModel->setSourceModel(SHORTCUT_MODEL);
    m_tableView->setModel(m_proxyModel);
    m_tableView->setSortingEnabled(true);
    m_proxyModel->sort(ShortcutModel::NameColumn, Qt::AscendingOrder);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->resizeColumnsToContents();
    m_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableView->verticalHeader()->hide();
    layout->addWidget(m_tableView);

    auto* createShortcutButton = new QPushButton(QIcon::fromTheme("list-add"), tr("Create shortcut"), this);
    connect(createShortcutButton, &QPushButton::clicked, this, &ShortcutsListWidget::onCreateShortcutClicked);
    layout->addWidget(createShortcutButton);
}

void ShortcutsListWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    const QModelIndexList selectedIndexes = m_tableView->selectionModel()->selectedRows();
    const QModelIndex currentIndex = m_proxyModel->mapToSource(m_tableView->selectionModel()->currentIndex());

    QAction* editAction = menu.addAction(QIcon::fromTheme("edit"), tr("Edit"));
    connect(editAction, &QAction::triggered, this, [this, currentIndex]() {
        QString exeFilePath = currentIndex.data(ShortcutModel::ExeFileRole).toString();
        Prefix* prefix = PREFIX_MODEL->forName(currentIndex.data(ShortcutModel::PrefixRole).toString());
        auto* editShortcutsDialog = new EditShortcutsDialog(exeFilePath, prefix, this);
        editShortcutsDialog->show();
    });

    QMenu* openMenu = menu.addMenu(QIcon::fromTheme("document-open-folder"), tr("Show"));

    QAction* openExeFileAction = openMenu->addAction(tr("Executable file"));
    connect(openExeFileAction, &QAction::triggered, this, [currentIndex]() {
        QString exeFilePath = QFileInfo(currentIndex.data(ShortcutModel::ExeFileRole).toString()).dir().path();
        QDesktopServices::openUrl(QUrl::fromLocalFile(exeFilePath));
    });

    QAction* openShortcutAction = openMenu->addAction(tr("Shortcut file"));
    connect(openShortcutAction, &QAction::triggered, this, [currentIndex]() {
        QString shortcutPath = QFileInfo(currentIndex.data(ShortcutModel::PathRole).toString()).dir().path();
        QDesktopServices::openUrl(QUrl::fromLocalFile(shortcutPath));
    });

    QAction* openPrefixAction = openMenu->addAction(tr("Prefix"));
    connect(openPrefixAction, &QAction::triggered, this, [this, currentIndex]() {
        QString prefixName = currentIndex.data(ShortcutModel::PrefixRole).toString();
        Prefix* prefix = PREFIX_MODEL->forName(prefixName);
        if (prefix != nullptr) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(prefix->path()));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("The \"%1\" prefix does not exist").arg(prefixName));
        }
    });

    menu.addSeparator();

    QAction* deleteAction = menu.addAction(QIcon::fromTheme("entry-delete"), tr("Delete"));
    connect(deleteAction, &QAction::triggered, this, [this, selectedIndexes]() {
        if (QMessageBox::question(this, tr("Confirm"), tr("Remove the selected shortcuts?")) == QMessageBox::Yes) {
            for (const auto& index : std::views::reverse(selectedIndexes)) {
                SHORTCUT_MODEL->removeRows(m_proxyModel->mapToSource(index).row(), 1);
            }
        }
    });

    menu.exec(event->globalPos());
}

void ShortcutsListWidget::onCreateShortcutClicked()
{
    QFileInfo exeFileInfo(
        QFileDialog::getOpenFileName(
            this,
            tr("Select the executable file"),
            QDir::homePath(),
            tr("Executable files (*.exe *.msi *.bat);;All files (*.*)")));

    if (exeFileInfo.exists()) {
        auto* editShortcutsDialog = new EditShortcutsDialog(exeFileInfo.absoluteFilePath(), nullptr, this);
        editShortcutsDialog->show();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("An unsuitable executable file has been selected"));
    }
}
