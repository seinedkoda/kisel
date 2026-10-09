#pragma once

#include "core/settings/base_settings.hpp"
#include "ui/compatibilitytools/old_device_info_widget.hpp"

namespace kisel {
class CompatibilitySettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit CompatibilitySettingsPage(BaseSettings* settings, QWidget* parent = nullptr);

private:
    BaseSettings* m_settings;
    OldDeviceInfoWidget* m_oldDeviceInfoWidget;
};
}