#pragma once

#include <QDir>
#include <QIcon>
#include <QObject>

namespace kisel {
class Ct : public QObject {
    Q_OBJECT

public:
    enum Status {
        Downloading,
        Unpacking,
        Installed,
        Unknown
    };
    Q_ENUM(Status);

    explicit Ct(const QString& path, QObject* parent = nullptr);

    [[nodiscard]] QString path() const;
    [[nodiscard]] QDir dir() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QIcon icon() const;
    void setStatus(Status status);
    [[nodiscard]] Status status() const;
    void setProgress(int progress);
    [[nodiscard]] int progress() const;
    [[nodiscard]] bool exists() const;
    static QString statusToString(Status status);

private:
    const QDir m_dir;
    QIcon m_icon;
    Status m_status;
    int m_progress;
};
}
