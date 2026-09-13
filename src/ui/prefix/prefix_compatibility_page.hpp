#pragma once

#include <QWidget>

#include "core/prefix/prefix_settings.hpp"
#include "ui/compatibilitytools/old_device_info_widget.hpp"

namespace kisel {
class PrefixCompatibilityPage : public QWidget {
    Q_OBJECT

public:
    explicit PrefixCompatibilityPage(PrefixSettings* settings, QWidget* parent = nullptr);

private:
    PrefixSettings* m_settings;
    OldDeviceInfoWidget* m_oldDeviceInfoWidget;
};
}