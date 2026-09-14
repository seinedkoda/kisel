#pragma once

#include <QTableView>

#include "core/shortcuts/shortcut_model.hpp"

namespace kisel {
class ShortcutsListWidget : public QWidget {
    Q_OBJECT

public:
    explicit ShortcutsListWidget(QWidget* parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onCreateShortcutClicked();

private:
    ShortcutProxyModel* m_proxyModel;
    QTableView* m_tableView;
};
}