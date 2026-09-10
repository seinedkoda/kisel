#include "shortcut_model.hpp"

#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QSaveFile>
#include <utility>

#include "core/appsettings/app_settings.hpp"
#include "core/shortcuts/shortcut.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

ShortcutModel::ShortcutModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    refreshList();
}

void ShortcutModel::refreshList()
{
    static const QList<QDir> dirs = Shortcut::shortcutDirs({ ShortcutLocation::Desktop | ShortcutLocation::Menu });
    for (const auto& dir : dirs) {
        if (!dir.exists()) {
            continue;
        }

        const QStringList desktopFileList = dir.entryList({ "*.kisel.desktop"_L1 }, QDir::Files);

        for (const QString& fileName : desktopFileList) {
            m_shortcuts.append(new Shortcut(dir.filePath(fileName)));
        }
    }
}

int ShortcutModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_shortcuts.count());
}

int ShortcutModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QHash<int, QByteArray> ShortcutModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "name" },
        { PathRole, "path" }
    };

    return roles;
}

Qt::ItemFlags ShortcutModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ShortcutModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (!index.isValid() || row >= m_shortcuts.count()) {
        return { };
    }

    int column = index.column();
    const Shortcut* shortcut = m_shortcuts.at(row);

    if (role == PathRole) {
        return shortcut->path();
    }

    if (column == Columns::Name) {
        switch (role) {
        case Qt::DisplayRole:
        case NameRole:
            return shortcut->name();
        case Qt::DecorationRole:
            return shortcut->icon();
        default:
            return { };
        }
    }

    if (column == Columns::Location && role == Qt::DisplayRole) {
        return shortcut->location() == ShortcutLocation::Menu ? tr("Menu") : tr("Desktop");
    }
    return { };
}

QVariant ShortcutModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return { };
    }

    switch (section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Location");
    default:
        return { };
    }
}

bool ShortcutModel::removeRows(int row, int count, const QModelIndex& parent) // NOLINT(bugprone-easily-swappable-parameters)
{
    if (row < 0 || row >= m_shortcuts.count()) {
        return false;
    }

    Shortcut* shortcut = m_shortcuts.at(row);
    if (!QFile::remove(shortcut->path())) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_shortcuts.removeAt(row);
    shortcut->deleteLater();
    endRemoveRows();
    return true;
}

void ShortcutModel::createShortcut(
    const ExecutableFile* exeFile,
    const Prefix* prefix,
    ShortcutLocations locations,
    QString shortcutName,
    QSize iconSize,
    const QString& category)
{
    if (!exeFile->isValid()) {
        return;
    }

    QString iconPath = saveHashedExeIconInPrefix(exeFile, iconSize, prefix);
    if (iconPath.isEmpty()) {
        qWarning() << "Failed to save icon for shortcut";
    }

    if (shortcutName.isEmpty()) {
        shortcutName = exeFile->baseName();
    }

    const QString escapedPrefixName = escapeExecArg(prefix->name());
    const QString escapedExePath = escapeExecArg(exeFile->path());

    QByteArray data;
    data += "[Desktop Entry]\n"_ba;
    data += "Type=Application\n"_ba;
    data += "Name="_ba % shortcutName.toUtf8() % u'\n';
    if (APP_SETTINGS->isFlatpak()) {
        data += "Exec=flatpak run --file-forwarding io.github.seinedkoda.kisel -p \""_ba
            % escapedPrefixName.toUtf8() % "\" @@ \""_ba
            % escapedExePath.toUtf8() % "\" @@\n"_ba;
    } else {
        data += "Exec=kisel -p \""_ba % escapedPrefixName.toUtf8() % "\" \""_ba % escapedExePath.toUtf8() % "\"\n"_ba;
    }
    data += "Icon="_ba % iconPath.toUtf8() % u'\n';
    data += "Categories="_ba % category.toUtf8() % ";\n"_ba;
    data += "StartupNotify=true\n"_ba;
    data += "Terminal=false\n"_ba;

    const QString shortcutFileName = exeFile->id() % ".kisel.desktop"_L1;

    static QMap<ShortcutLocation, QDir> dirMap {
        { ShortcutLocation::Desktop, Shortcut::desktopShortcutsDir() },
        { ShortcutLocation::Menu, Shortcut::menuShortcutsDir() }
    };

    for (auto i = dirMap.cbegin(), end = dirMap.cend(); i != end; ++i) {
        if (!locations.testFlag(i.key())) {
            continue;
        }

        const QDir& dir = i.value();

        if (!dir.exists() && !dir.mkpath(".")) {
            qCritical() << "Failed to create shortcut directory:" << dir.path();
            continue;
        }

        QString destPath = dir.filePath(shortcutFileName);
        QSaveFile file(destPath);
        if (!file.open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to open the file for writing:" << destPath;
        }

        if (file.write(data) != data.size()) {
            qCritical() << "Failed to write data to the file:" << file.errorString();
            return;
        }

        if (!file.commit()) {
            qCritical() << "Failed to save changes to the file:" << file.errorString();
            return;
        }

        if (!file.setPermissions(file.permissions() | QFileDevice::ExeUser)) {
            qWarning() << "Failed to make .desktop file executable:" << file.errorString();
        }

        Shortcut* sh = shortcut(exeFile->id(), i.key());
        if (sh == nullptr) {
            int insertPos = rowCount();
            beginInsertRows(QModelIndex(), insertPos, insertPos);
            m_shortcuts.insert(insertPos, new Shortcut(destPath));
            endInsertRows();
        } else {
            sh->parseData();
            QModelIndex idx = index(static_cast<int>(m_shortcuts.indexOf(sh)), 0);
            emit dataChanged(idx, idx, { NameRole });
        }
    }
}

QString ShortcutModel::saveHashedExeIconInPrefix(const ExecutableFile* exeFile, QSize iconSize, const Prefix* prefix)
{
    const QIcon& icon = exeFile->icon();
    if (icon.isNull()) {
        return { };
    }

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);

    if (iconSize.isNull()) {
        iconSize = QSize(256, 256);
    }

    const QSize size = icon.actualSize(iconSize);
    const QPixmap& pixmap = icon.pixmap(size);

    if (!pixmap.save(&buffer, "PNG")) {
        return { };
    }

    const QByteArray hashBytes = QCryptographicHash::hash(bytes, QCryptographicHash::Md5);
    QString hashString = QString::fromLatin1(hashBytes.toHex());

    const QDir iconsDir(prefix->dir().filePath(".kisel/icons/"_L1));
    if (!iconsDir.exists() && !iconsDir.mkpath(".")) {
        qCritical() << "Failed to create prefix icons directory:" << iconsDir.path();
        return { };
    }

    QString filePath = iconsDir.filePath(exeFile->name() % "_"_L1 % hashString % ".png"_L1);
    if (pixmap.save(filePath, "PNG")) {
        return filePath;
    }

    return { };
}

QString ShortcutModel::escapeExecArg(QString arg)
{
    arg.replace(u'\\', R"(\\)"_L1);
    arg.replace(u'"', R"(\")"_L1);
    arg.replace(u'%', R"(%%)"_L1);
    return arg;
}

Shortcut* ShortcutModel::shortcut(QStringView exeId, ShortcutLocation location)
{
    for (Shortcut* shortcut : std::as_const(m_shortcuts)) {
        if (shortcut->id() == exeId && shortcut->location() == location) {
            return shortcut;
        }
    }
    return nullptr;
}

void ShortcutModel::removeShortcut(Shortcut* removableShortcut)
{
    for (int row = 0; row < m_shortcuts.count(); row++) {
        const Shortcut* shortcut = m_shortcuts.at(row);
        if (shortcut == removableShortcut) {
            removeRows(row, 1);
        }
    }
}
