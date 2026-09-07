#pragma once

#include <QFileInfo>
#include <QIcon>

namespace kisel {
class ExecutableFile : public QObject {
    Q_OBJECT

public:
    explicit ExecutableFile(const QString& path = "", QObject* parent = nullptr);

    void setPath(const QString& newPath);
    [[nodiscard]] QString id() const;
    [[nodiscard]] QString path() const;
    [[nodiscard]] QString dirPath() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString baseName() const;
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isMsi() const;
    [[nodiscard]] bool isCmd() const;
    [[nodiscard]] const QIcon& icon() const;

private:
    void setIdFromPath();
    void loadIcon();
    [[nodiscard]] QString findBestIconGroupName() const;
    [[nodiscard]] bool extractIconGroup(const QString& groupName, const QString& outputPath) const;

    QString m_id;
    QFileInfo m_fileInfo;
    QIcon m_icon;
};
}