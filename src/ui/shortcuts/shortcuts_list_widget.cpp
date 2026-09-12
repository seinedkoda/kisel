#include "shortcuts_list_widget.hpp"

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>

#include "core/app/app.hpp"
#include "core/shortcuts/shortcut_model.hpp"

using namespace kisel;

ShortcutsListWidget::ShortcutsListWidget(QWidget* parent)
    : QTableView(parent)
    , m_proxyModel(new ShortcutProxyModel(this))
{
    m_proxyModel->setSourceModel(SHORTCUT_MODEL);
    setModel(m_proxyModel);
    setSortingEnabled(true);
    m_proxyModel->sort(ShortcutModel::NameColumn, Qt::AscendingOrder);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    resizeColumnsToContents();
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    verticalHeader()->hide();
}

void ShortcutsListWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    const QModelIndexList selectedIndexes = selectionModel()->selectedRows();
    const QModelIndex currentIndex = m_proxyModel->mapToSource(selectionModel()->currentIndex());

    QAction* openAction = menu.addAction(QIcon::fromTheme("document-open-folder"), tr("Open location"));
    connect(openAction, &QAction::triggered, this, [currentIndex]() {
        QString location = QFileInfo(currentIndex.data(ShortcutModel::PathRole).toString()).dir().path();
        QDesktopServices::openUrl(QUrl::fromLocalFile(location));
    });

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
