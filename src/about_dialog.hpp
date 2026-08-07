#pragma once

#include <QDialog>

namespace kisel {
class AboutDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr);
};
}