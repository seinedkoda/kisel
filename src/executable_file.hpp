#pragma once

#include <QFileInfo>
#include <QProcess>
#include <QString>

#include "prefix.hpp"

namespace kisel {
enum class ShortcutDestination {
    Menu,
    Desktop
};

class ExecutableFile : public QObject {
    Q_OBJECT

public:
    explicit ExecutableFile(const QString& path, QObject* parent = nullptr);

    void setPath(const QString& newPath);
    [[nodiscard]] QString path() const;
    [[nodiscard]] QString dirPath() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString baseName() const;
    [[nodiscard]] Prefix* prefix() const;
    [[nodiscard]] bool isValid() const;
    const QIcon& icon();
    void setPrefix(Prefix* prefix);
    void setPrefixName(const QString& prefixName);
    void createShortcut(
        QString shortcutName = "",
        ShortcutDestination shortcutDest = ShortcutDestination::Menu,
        const QString& category = "");

private:
    void loadIcon();
    [[nodiscard]] QString findBestIconGroupName() const;
    [[nodiscard]] bool extractIconGroup(const QString& groupName, const QString& outputPath) const;
    QString saveIconWithHashName(const QDir& outputDir);

    QFileInfo m_fileInfo;
    QIcon m_icon;
    QPointer<Prefix> m_prefix = nullptr;
    bool m_needUpdateIcon;
};
}