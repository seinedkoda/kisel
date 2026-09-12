#pragma once

#include <QDir>
#include <QIcon>
#include <QObject>

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
    [[nodiscard]] QString prefixName() const;
    [[nodiscard]] QString exeFilePath() const;
    [[nodiscard]] QString category() const;
    [[nodiscard]] ShortcutLocation location() const;
    [[nodiscard]] const QIcon& icon() const;

private:
    void parseExecValue(QStringView value);

    QString m_id;
    QString m_path;
    QString m_name;
    QString m_prefixName;
    QString m_exeFilePath;
    QString m_category;
    QIcon m_icon;
    ShortcutLocation m_location;
};
}
