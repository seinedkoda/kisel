#pragma once

#include <QWidget>

namespace kisel {
class PrefixPage : public QWidget {
    Q_OBJECT

public:
    explicit PrefixPage(QWidget* parent = nullptr);
};
}