#pragma once

#include <QTableView>

namespace kisel {
class ShortcutsListWidget : public QTableView {
    Q_OBJECT

public:
    explicit ShortcutsListWidget(QWidget* parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};
}