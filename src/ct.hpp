#pragma once

#include <QDir>
#include <QIcon>
#include <QObject>

namespace kisel {
class Ct : public QObject {
    Q_OBJECT

public:
    explicit Ct(const QString& path, QObject* parent = nullptr);

    [[nodiscard]] QString path() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QIcon icon() const;
    [[nodiscard]] bool exists() const;

private:
    const QDir m_dir;
    QIcon m_icon;
};
}
