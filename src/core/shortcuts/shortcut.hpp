#pragma once

#include <QObject>
#include <QIcon>
#include <QDir>

namespace kisel {
enum ShortcutLocation {
    Desktop = 0x1,
    Menu = 0x2
};
Q_DECLARE_FLAGS(ShortcutLocations, ShortcutLocation)

class Shortcut : public QObject {
public:
    explicit Shortcut(const QString& filePath, QObject* parent = nullptr);

    static QList<QDir> shortcutDirs(ShortcutLocations locations);
    static QDir desktopShortcutsDir();
    static QDir menuShortcutsDir();

    void parseData();
    [[nodiscard]] QString id() const;
    [[nodiscard]] QString path() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] ShortcutLocation location() const;
    [[nodiscard]] const QIcon& icon() const;

private:
    QString m_id;
    QString m_path;
    QString m_name;
    QIcon m_icon;
    ShortcutLocation m_location;
};
}
