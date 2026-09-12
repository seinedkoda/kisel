#pragma once

#include <QAbstractTableModel>
#include <QIcon>
#include <QSortFilterProxyModel>
#include <QCollator>

#include "core/executablefile/executable_file.hpp"
#include "core/prefix/prefix.hpp"
#include "core/shortcuts/shortcut.hpp"

namespace kisel {
class ShortcutModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole
    };
    Q_ENUM(Roles)

    enum Columns {
        NameColumn = 0,
        LocationColumn = 1
    };
    Q_ENUM(Columns)

    explicit ShortcutModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    void createShortcut(
        const ExecutableFile* exeFile,
        const Prefix* prefix,
        ShortcutLocations locations,
        QString shortcutName = "",
        QSize iconSize = { },
        const QString& category = "");
    void refreshList();
    Shortcut* shortcut(QStringView exeId, ShortcutLocation location);
    void removeShortcut(Shortcut* removableShortcut);

private:
    static QString saveHashedExeIconInPrefix(const ExecutableFile* exeFile, QSize iconSize, const Prefix* prefix);
    static QString escapeExecArg(QString arg);

    QList<Shortcut*> m_shortcuts;
};

class ShortcutProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit ShortcutProxyModel(QObject* parent = nullptr);

protected:
    [[nodiscard]] bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;

private:
    QCollator m_collator;
};
}