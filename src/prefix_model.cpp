#include "prefix_model.hpp"

#include <QDir>
#include <QRegularExpression>

#include "app_settings.hpp"

using namespace kisel;

PrefixModel::PrefixModel(QObject* parent)
    : QAbstractListModel(parent)
{
    if (!PREFIXES_DIR.exists() && !PREFIXES_DIR.mkpath(".")) {
        qCritical() << "Unable to create root directory for prefixes";
        return;
    }

    refreshList();
}

PrefixModel* PrefixModel::instance()
{
    static PrefixModel instance;
    return &instance;
}

int PrefixModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_prefixes.count());
}

QHash<int, QByteArray> PrefixModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "name" },
        { PathRole, "path" }
    };

    return roles;
}

Qt::ItemFlags PrefixModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PrefixModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (!index.isValid() || row >= m_prefixes.count()) {
        return { };
    }

    const Prefix* prefix = m_prefixes.at(row);

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return prefix->name();
    case PathRole:
        return prefix->path();
    default:
        return { };
    }
}

bool PrefixModel::removeRows(int row, int count, const QModelIndex& parent) // NOLINT(bugprone-easily-swappable-parameters)
{
    if (row < 0 || row >= m_prefixes.count()) {
        return false;
    }

    Prefix* prefix = m_prefixes.at(row);
    if (prefix->exists() && !prefix->dir().removeRecursively()) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_prefixes.removeAt(row);
    prefix->deleteLater();
    endRemoveRows();
    return true;
}

Prefix* PrefixModel::forIndex(int index) const
{
    if (index >= 0 && index < m_prefixes.count()) {
        return m_prefixes.at(index);
    }

    return nullptr;
}

Prefix* PrefixModel::forPath(QStringView path) const
{
    if (path.isEmpty()) {
        return nullptr;
    }

    for (auto* prefix : std::as_const(m_prefixes)) {
        if (path == prefix->path()) {
            return prefix;
        }
    }

    return nullptr;
}

Prefix* PrefixModel::forName(QStringView name) const
{
    if (name.isEmpty()) {
        return nullptr;
    }

    for (auto* prefix : std::as_const(m_prefixes)) {
        if (name == prefix->name()) {
            return prefix;
        }
    }

    return nullptr;
}

void PrefixModel::refreshList()
{
    const QStringList dirList = PREFIXES_DIR.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Add new prefixes from dir
    for (const QString& dirName : dirList) {
        if (!containsName(dirName)) {
            add(dirName);
        }
    }

    // Safely remove non-existent prefixes in reverse order
    for (int i = rowCount() - 1; i >= 0; --i) {
        Prefix* prefix = m_prefixes.at(i);

        if (!dirList.contains(prefix->name())) {
            beginRemoveRows(QModelIndex(), i, i);
            m_prefixes.removeAt(i);
            prefix->deleteLater();
            endRemoveRows();
        }
    }

    if (m_prefixes.count() == 0) {
        add(APP_SETTINGS->defaultPrefixName());
    }
}

Prefix* PrefixModel::add(const QString& name)
{
    if (!isValidPrefixName(name)) {
        return nullptr;
    }

    auto* prefix = new Prefix(name, this);

    int insertPos = rowCount();
    beginInsertRows(QModelIndex(), insertPos, insertPos);
    m_prefixes.insert(insertPos, prefix);
    endInsertRows();
    return prefix;
}

Prefix* PrefixModel::defaultPrefix()
{
    QString defaultPrefixName = APP_SETTINGS->defaultPrefixName();
    Prefix* defaultPrefix = forName(defaultPrefixName);

    if (defaultPrefix == nullptr) {
        qWarning() << "The default prefix does not exist";
        return add(defaultPrefixName);
    }

    return defaultPrefix;
}

bool PrefixModel::isValidPrefixName(QStringView name)
{
    if (name.isEmpty()) {
        return false;
    }

    static const QRegularExpression nameRegex(QStringLiteral(R"(^(?!\.$)(?!\.\.$)[^\/\x00-\x1F\x7F]+$)")); // POSIX directory
    if (!nameRegex.matchView(name).hasMatch()) {
        return false;
    }

    return !containsName(name);
}

bool PrefixModel::containsName(QStringView name)
{
    return std::ranges::any_of(m_prefixes, [&name](const QString& prefixName) { return prefixName == name; }, &Prefix::name);
}

const QList<Prefix*>& PrefixModel::list() const
{
    return m_prefixes;
}