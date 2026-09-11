#pragma once

#include <QComboBox>
#include <QDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QToolButton>

namespace kisel {
class CtListWidget : public QWidget {
    Q_OBJECT

public:
    explicit CtListWidget(QWidget* parent = nullptr);

private slots:
    void openAddNewCtDialog();
    void onInstallationError(const QString& errorText);
    void onContextMenuRequested(const QPoint& pos);

private:
    QTableView* m_ctTableView;
};

class ProgressBarDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
}