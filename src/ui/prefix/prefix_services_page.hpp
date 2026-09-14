#pragma once

#include <QWidget>

#include "core/prefix/prefix_settings.hpp"

namespace kisel {
class PrefixServicesPage : public QWidget {
    Q_OBJECT

public:
    explicit PrefixServicesPage(PrefixSettings* settings, QWidget* parent = nullptr);

private:
    PrefixSettings* m_settings;
};
}