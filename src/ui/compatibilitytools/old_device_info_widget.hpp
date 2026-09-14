#pragma once

#include <QWidget>

namespace kisel {
class OldDeviceInfoWidget : public QWidget {
    Q_OBJECT
public:
    explicit OldDeviceInfoWidget(QWidget* parent = nullptr);

    static bool isCompatibleCt(const QString& ctName);
};
}