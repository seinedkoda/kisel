#pragma once

#include <QListView>
#include <QWidget>

namespace kisel {
class PrefixListWidget : public QWidget {
    Q_OBJECT
public:
    explicit PrefixListWidget(QWidget* parent = nullptr);

private slots:
    void onContextMenuRequested(const QPoint& pos);

private:
    QListView* m_prefixListView;
};
}