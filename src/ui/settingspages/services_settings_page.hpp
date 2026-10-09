#pragma once

#include <QWidget>

#include "core/settings/base_settings.hpp"

namespace kisel {
class ServicesSettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit ServicesSettingsPage(BaseSettings* settings, QWidget* parent = nullptr);

private:
    BaseSettings* m_settings;
};
}