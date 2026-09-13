#pragma once

#include <QWidget>

#include "ui/compatibilitytools/old_device_info_widget.hpp"

namespace kisel {
class CtPage : public QWidget {
    Q_OBJECT

public:
    explicit CtPage(QWidget* parent = nullptr);

private:
    OldDeviceInfoWidget* m_oldDeviceInfoWidget;
};
}