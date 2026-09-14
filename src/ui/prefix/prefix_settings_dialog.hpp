#pragma once

#include <QDialog>

#include "core/prefix/prefix.hpp"

namespace kisel {
class PrefixSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit PrefixSettingsDialog(Prefix* prefix, QWidget* parent = nullptr);

private:
    PrefixSettings* m_settings;
};
}