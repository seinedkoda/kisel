#pragma once

#include <QDir>
#include <QObject>

#include "core/settings/prefix_settings.hpp"

namespace kisel {
class Prefix : public QObject {
    Q_OBJECT

public:
    explicit Prefix(const QString& path, QObject* parent = nullptr);

    [[nodiscard]] QString path() const;
    [[nodiscard]] QDir dir() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] PrefixSettings* settings() const;
    [[nodiscard]] bool exists() const;
    bool makePath();

private:
    const QDir m_dir;
    const QString m_name;
    PrefixSettings* m_settings;
};
}