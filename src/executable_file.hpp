#pragma once

#include "run_config.hpp"

namespace kisel {
class ExecutableFile : public QObject {
    Q_OBJECT

public:
    enum ShortcutDestination {
        Menu,
        Desktop
    };
    Q_ENUM(ShortcutDestination)

    explicit ExecutableFile(const QString& path = "", QObject* parent = nullptr);

    void setPath(const QString& newPath);
    [[nodiscard]] QString path() const;
    [[nodiscard]] QString dirPath() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString baseName() const;
    [[nodiscard]] bool isValid() const;
    const QIcon& icon();
    void createShortcut(
        const Prefix& prefix,
        QString shortcutName = "",
        ShortcutDestination shortcutDest = Menu,
        const QString& category = "") const;

private:
    void loadIcon();
    [[nodiscard]] QString findBestIconGroupName() const;
    [[nodiscard]] bool extractIconGroup(const QString& groupName, const QString& outputPath) const;
    [[nodiscard]] QString saveIconWithHashName(const QDir& outputDir) const;

    QFileInfo m_fileInfo;
    QIcon m_icon;
    bool m_needUpdateIcon;
};
}