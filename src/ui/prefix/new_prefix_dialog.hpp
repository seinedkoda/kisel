#pragma once

#include <QDialog>
#include <QLineEdit>

namespace kisel {
class NewPrefixDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewPrefixDialog(QWidget* parent = nullptr);

private:
    QLineEdit* m_nameInput;
    QPushButton* m_saveButton;
};
}