#pragma once

#include <QWidget>

#include "core/settings/app_settings.hpp"

namespace kisel {
class AppPlatformSettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit AppPlatformSettingsPage(AppSettings* settings, QWidget* parent = nullptr);

private:
    AppSettings* m_settings;
};
}