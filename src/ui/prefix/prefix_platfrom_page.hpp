#pragma once

#include <QWidget>

#include "core/prefix/prefix_settings.hpp"

namespace kisel {
class PrefixPlatformPage : public QWidget {
    Q_OBJECT

public:
    explicit PrefixPlatformPage(PrefixSettings* settings, QWidget* parent = nullptr);

private:
    static const QStringList& storeList();
};
}