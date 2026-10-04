#pragma once

#include <QAbstractListModel>

#include "prefix.hpp"

namespace kisel {
class PrefixModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole
    };
    Q_ENUM(Roles)

    explicit PrefixModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    [[nodiscard]] Prefix* getByIndex(int index) const;
    [[nodiscard]] Prefix* getByPath(QStringView path) const;
    [[nodiscard]] Prefix* getByName(QStringView name) const;
    [[nodiscard]] const QList<Prefix*>& list() const;
    void refreshList();
    Prefix* add(const QString& path);
    Prefix* defaultPrefix();
    bool isValidPrefixName(QStringView name);
    bool containsName(QStringView name);

private:
    QList<Prefix*> m_prefixes;
};
}
