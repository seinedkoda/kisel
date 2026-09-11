#pragma once

#include <QWidget>

namespace kisel {
class CtPage : public QWidget {
    Q_OBJECT

public:
    explicit CtPage(QWidget* parent = nullptr);
};
}