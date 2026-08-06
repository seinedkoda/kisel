#pragma once

#include <QObject>
#include <QPointer>

#include "ct.hpp"
#include "prefix.hpp"

namespace kisel {
class RunConfig : public QObject {
    Q_OBJECT

public:
    explicit RunConfig(QObject* parent = nullptr);

    void setPrefix(Prefix* prefix);
    void setPrefixName(const QString& prefixName);
    [[nodiscard]] Prefix* prefix() const;
    void setCt(Ct* ct);
    [[nodiscard]] Ct* ct() const;

private:
    QPointer<Prefix> m_prefix;
    QPointer<Ct> m_ct;
};
}