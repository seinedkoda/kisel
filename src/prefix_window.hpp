#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListView>
#include <QMainWindow>
#include <QPushButton>

namespace kisel {
class AddNewPrefixDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddNewPrefixDialog(QWidget* parent = nullptr);

private:
    QLineEdit* m_nameInput;
    QPushButton* m_saveButton;
};

class PrefixWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit PrefixWindow(QWidget* parent = nullptr);
private slots:
    void onContextMenuRequested(const QPoint& pos);

private:
    QListView* m_prefixListView;
    QMenu* m_listViewContextMenu;
    QAction* m_openAction;
    QAction* m_deleteAction;
};
}