#pragma once

#include <QWidget>

#include "core/settings/prefix_settings.hpp"

namespace kisel {
class PlatformSettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit PlatformSettingsPage(PrefixSettings* prefixSettings, QWidget* parent = nullptr);

private:
    static const QStringList& storeList();

    PrefixSettings* m_settings;
};
}