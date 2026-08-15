#include "ct_model.hpp"

#include <QTemporaryFile>

#include "app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

CtModel::CtModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    refreshList();
}

CtModel* CtModel::instance()
{
    static CtModel instance;
    return &instance;
}

int CtModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_cts.count());
}

int CtModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 2;
}

Qt::ItemFlags CtModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant CtModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (!index.isValid() || row >= m_cts.count()) {
        return { };
    }

    const Ct* ct = m_cts.at(row);

    int column = index.column();
    if (column == 0) {
        switch (role) {
        case Qt::DisplayRole:
            return ct->name();
        case Qt::DecorationRole:
            return ct->icon();
        default:
            return { };
        }
    } else if (column == 1) {
        switch (role) {
        case Qt::DisplayRole:
            return Ct::statusToString(ct->status());
        case StatusRole:
            return ct->status();
        case ProgressRole:
            return ct->progress();
        default:
            return { };
        }
    }
    return { };
}

QHash<int, QByteArray> CtModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "name" },
        { PathRole, "path" },
        { StatusRole, "status" },
        { ProgressRole, "progress" }
    };

    return roles;
}

QVariant CtModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return { };
    }

    switch (section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Status");
    default:
        return { };
    }
}

bool CtModel::removeRows(int row, int count, const QModelIndex& parent) // NOLINT(bugprone-easily-swappable-parameters)
{
    if (row < 0 || row >= m_cts.count()) {
        return false;
    }

    Ct* ct = m_cts.at(row);
    if (ct->exists() && !ct->dir().removeRecursively()) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_cts.removeAt(row);
    ct->deleteLater();
    endRemoveRows();
    return true;
}

int CtModel::ctIndex(Ct* ct) const
{
    if (ct == nullptr) {
        return -1;
    }

    return static_cast<int>(m_cts.indexOf(ct));
}

Ct* CtModel::forIndex(int index) const
{
    if (index >= 0 && index < m_cts.count()) {
        return m_cts.at(index);
    }

    return nullptr;
}

Ct* CtModel::forPath(QStringView path) const
{
    if (path.isEmpty()) {
        return nullptr;
    }

    for (auto* ct : std::as_const(m_cts)) {
        if (path == ct->path()) {
            return ct;
        }
    }

    return nullptr;
}

void CtModel::refreshList()
{
    for (const auto& ctsDir : CTS_DIR_LIST) {
        if (!ctsDir.exists()) {
            continue;
        }

        const auto entryInfoList = ctsDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        // Add new ones
        for (const auto& fileInfo : entryInfoList) {
            QString ctPath = fileInfo.absoluteFilePath();
            if (!containsPath(ctPath) && QFileInfo::exists(ctPath % "/proton"_L1)) {
                add(ctPath);
            }
        }

        // Safely remove non-existent ct in reverse order
        for (int i = rowCount() - 1; i >= 0; --i) {
            Ct* ct = m_cts.at(i);

            if (ct->status() == Ct::Installed && !ct->exists()) {
                beginRemoveRows(QModelIndex(), i, i);
                m_cts.removeAt(i);
                ct->deleteLater();
                endRemoveRows();
            }
        }
    }
}

Ct* CtModel::add(const QString& path)
{
    if (path.isEmpty()) {
        return nullptr;
    }

    Ct* ct = new Ct(path, this);

    int insertPos = rowCount();
    beginInsertRows(QModelIndex(), insertPos, insertPos);
    m_cts.insert(insertPos, ct);
    endInsertRows();
    return ct;
}

bool CtModel::containsPath(QStringView path)
{
    return std::ranges::any_of(m_cts, [&path](const QString& ctPath) { return ctPath == path; }, &Ct::path);
}

Ct* CtModel::defaultCt()
{
    const QString defaultCtPath = APP_SETTINGS->defaultCtPath();
    if (!defaultCtPath.isEmpty() && QFileInfo::exists(defaultCtPath)) {
        return forPath(defaultCtPath);
    }

    if (!m_cts.isEmpty()) {
        return m_cts.first();
    }

    return nullptr;
}

void CtModel::setCtDownloadProgress(Ct* ct, qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        if (ct->progress() == percentage) {
            return;
        }

        ct->setProgress(percentage);
        QModelIndex modelIndex = index(ctIndex(ct), 1);
        emit dataChanged(modelIndex, modelIndex, { ProgressRole });
    }
}

void CtModel::setCtStatus(Ct* ct, Ct::Status status)
{
    ct->setStatus(status);
    QModelIndex modelIndex = index(ctIndex(ct), 1);
    emit dataChanged(modelIndex, modelIndex, { Qt::DisplayRole, StatusRole });
}

bool CtInstalledProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
    auto status = index.data(CtModel::StatusRole).value<Ct::Status>();
    return status == Ct::Installed;
}
