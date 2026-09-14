#pragma once

#include <QMainWindow>

namespace kisel {
class AppSettingsWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AppSettingsWindow(QWidget* parent = nullptr);
};
}