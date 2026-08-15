#pragma once

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <qsortfilterproxymodel.h>

#include "ct.hpp"

namespace kisel {
#define CT_MODEL CtModel::instance()

class CtModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        StatusRole,
        ProgressRole
    };
    Q_ENUM(Roles);

    static CtModel* instance();

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    [[nodiscard]] int ctIndex(Ct* ct) const;
    [[nodiscard]] Ct* forIndex(int index) const;
    [[nodiscard]] Ct* forPath(QStringView path) const;
    void refreshList();
    Ct* add(const QString& path);
    Ct* defaultCt();
    void setCtDownloadProgress(Ct* ct, qint64 bytesReceived, qint64 bytesTotal);
    void setCtStatus(Ct* ct, Ct::Status status);

private:
    explicit CtModel(QObject* parent = nullptr);

    bool containsPath(QStringView path);

    QList<Ct*> m_cts;
};

class CtInstalledProxyModel : public QSortFilterProxyModel {
Q_OBJECT
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    [[nodiscard]] bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
};
}
