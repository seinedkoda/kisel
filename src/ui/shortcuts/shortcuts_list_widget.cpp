#include "shortcuts_list_widget.hpp"

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>

#include "core/app/app.hpp"

using namespace kisel;

ShortcutsListWidget::ShortcutsListWidget(QWidget* parent)
    : QTableView(parent)
{
    setModel(SHORTCUT_MODEL);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    resizeColumnsToContents();
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    verticalHeader()->hide();
}

void ShortcutsListWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    const QModelIndexList selectedRows = selectionModel()->selectedRows();
    const QModelIndex currentIndex = selectionModel()->currentIndex();

    QAction* openAction = menu.addAction(QIcon::fromTheme("document-open-folder"), tr("Open location"));
    connect(openAction, &QAction::triggered, this, [currentIndex]() {
        QString location = QFileInfo(currentIndex.data(ShortcutModel::PathRole).toString()).dir().path();
        QDesktopServices::openUrl(QUrl::fromLocalFile(location));
    });

    QAction* deleteAction = menu.addAction(QIcon::fromTheme("entry-delete"), tr("Delete"));
    connect(deleteAction, &QAction::triggered, this, [this, selectedRows]() {
        if (QMessageBox::question(this, tr("Confirm"), tr("Remove the selected shortcuts?")) == QMessageBox::Yes) {
            for (const auto& index : std::views::reverse(selectedRows)) {
                SHORTCUT_MODEL->removeRows(index.row(), 1);
            }
        }
    });

    menu.exec(event->globalPos());
}
