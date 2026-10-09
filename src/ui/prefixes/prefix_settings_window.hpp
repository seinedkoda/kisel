#pragma once

#include <QWidget>

#include "core/prefix/prefix.hpp"

namespace kisel {
class PrefixSettingsWindow : public QWidget {
    Q_OBJECT

public:
    explicit PrefixSettingsWindow(Prefix* prefix, QWidget* parent = nullptr);

private:
    PrefixSettings* m_settings;
};
}