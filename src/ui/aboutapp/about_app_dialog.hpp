#pragma once

#include <QDialog>

namespace kisel {
class AboutAppDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutAppDialog(QWidget* parent = nullptr);
};
}