#pragma once

#include <QAbstractListModel>
#include <qabstractitemmodel.h>

#include "prefix.hpp"

namespace kisel {
#define PREFIX_MODEL PrefixModel::instance()

class PrefixModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole
    };
    Q_ENUM(Roles)

    static PrefixModel* instance();

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    [[nodiscard]] Prefix* forIndex(int index) const;
    [[nodiscard]] Prefix* forPath(QStringView path) const;
    [[nodiscard]] Prefix* forName(QStringView name) const;
    [[nodiscard]] const QList<Prefix*>& list() const;
    void refreshList();
    Prefix* add(const QString& name);
    Prefix* defaultPrefix();
    bool isValidPrefixName(QStringView name);
    bool containsName(QStringView name);

private:
    explicit PrefixModel(QObject* parent = nullptr);

    QList<Prefix*> m_prefixes;
};
}
