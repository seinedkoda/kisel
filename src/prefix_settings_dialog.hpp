#pragma once

#include <QDialog>
#include <QCheckBox>
#include <QObject>

#include "prefix.hpp"

namespace kisel {
class PrefixSettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit PrefixSettingsDialog(Prefix* prefix, QWidget* parent = nullptr);
private:
    Prefix* m_prefix;
};
}