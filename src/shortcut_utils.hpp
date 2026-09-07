#pragma once

#include <QFlags>

#include "executable_file.hpp"
#include "prefix.hpp"

namespace kisel {
enum ShortcutLocation {
    Desktop = 0x1,
    Menu = 0x2
};

Q_DECLARE_FLAGS(ShortcutLocations, ShortcutLocation)

void createShortcut(
    const ExecutableFile* exeFile,
    const Prefix* prefix,
    ShortcutLocations locations,
    QString shortcutName = "",
    QSize iconSize = { },
    const QString& category = "");
}