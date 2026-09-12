#pragma once

#include <QTableView>

#include "core/shortcuts/shortcut_model.hpp"

namespace kisel {
class ShortcutsListWidget : public QTableView {
    Q_OBJECT

public:
    explicit ShortcutsListWidget(QWidget* parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    ShortcutProxyModel* m_proxyModel;
};
}