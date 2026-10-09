#pragma once

#include <QWidget>

namespace kisel {
class AppSettingsGeneralPage : public QWidget {
    Q_OBJECT
public:
    explicit AppSettingsGeneralPage(QWidget* parent = nullptr);
};
}