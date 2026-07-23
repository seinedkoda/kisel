#pragma once

#include <QDir>
#include <QObject>
#include <QPointer>
#include <QSettings>

#include "ct.hpp"
#include "prefix_settings.hpp"

namespace kisel {
class Prefix : public QObject {
    Q_OBJECT

public:
    explicit Prefix(const QString& path, QObject* parent = nullptr);

    [[nodiscard]] QString path() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] Ct* ct() const;
    [[nodiscard]] PrefixSettings* settings() const;
    bool makePath();
    void setCt(Ct* ct);

private:
    const QDir m_dir;
    PrefixSettings* m_settings;
    QPointer<Ct> m_ct;
};
}