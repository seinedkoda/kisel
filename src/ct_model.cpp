#include "ct_model.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryFile>

#include "app_settings.hpp"

using namespace kisel;

CtModel::CtModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_ctSourceName("Proton-GE (Github)")
{
    refreshList();
}

CtModel::~CtModel()
{
    if (m_installationIsRunning) {
        cancelInstallation();
    }
}

CtModel* CtModel::instance()
{
    static CtModel instance;
    return &instance;
}

const QMap<QString, QUrl>& CtModel::ctSourceMap()
{
    static QMap<QString, QUrl> map {
        { "Proton-GE (Github)", QUrl("https://api.github.com/repos/GloriousEggroll/proton-ge-custom/releases?per_page=10") },
        { "Proton-CachyOS (Github)", QUrl("https://api.github.com/repos/CachyOS/proton-cachyos/releases?per_page=10") }
    };
    return map;
}

int CtModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_cts.count());
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

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return ct->name();
    case PathRole:
        return ct->path();
    case Qt::DecorationRole:
        return ct->icon();
    default:
        return { };
    }
}

QHash<int, QByteArray> CtModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "name" },
        { PathRole, "path" }
    };

    return roles;
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

QStringList CtModel::availableReleasesList() const
{
    return m_sortedReleasesList;
}

QString CtModel::ctSourceName() const
{
    return m_ctSourceName;
}

bool CtModel::installationIsRunning() const
{
    return m_installationIsRunning;
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
            if (!containsPath(ctPath)) {
                add(ctPath);
            }
        }

        // Safely remove non-existent ct in reverse order
        for (int i = rowCount() - 1; i >= 0; --i) {
            Ct* ct = m_cts.at(i);

            if (!ct->exists()) {
                beginRemoveRows(QModelIndex(), i, i);
                m_cts.removeAt(i);
                ct->deleteLater();
                endRemoveRows();
            }
        }
    }
}

void CtModel::add(const QString& path)
{
    if (!QFileInfo::exists(path)) {
        return;
    }

    Ct* ct = new Ct(path, this);

    int insertPos = rowCount();
    beginInsertRows(QModelIndex(), insertPos, insertPos);
    m_cts.insert(insertPos, ct);
    endInsertRows();
}

void CtModel::remove(const QModelIndex& itemIndex)
{
    if (!itemIndex.isValid()) {
        return;
    }

    int row = itemIndex.row();
    QString dirPath = itemIndex.data(PathRole).toString();
    if (QDir(dirPath).removeRecursively()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_cts.remove(row);
        endRemoveRows();
    }
}

void CtModel::setCtSourceFromName(const QString& name)
{
    if (ctSourceMap().contains(name)) {
        m_ctSourceName = name;
    }
}

bool CtModel::containsPath(QStringView path)
{
    return std::ranges::any_of(m_cts, [&path](const QString& ctPath) { return ctPath == path; }, &Ct::path);
}

void CtModel::cancelInstallation()
{
    if (!m_installationIsRunning) {
        return;
    }

    if (m_downloadReply != nullptr) {
        m_downloadReply->abort();
        m_downloadReply->deleteLater();
    }

    if (m_tarProcess != nullptr) {
        m_tarProcess->terminate();
        m_tarProcess->waitForFinished();
        m_tarProcess->deleteLater();
    }

    emit installationCanceled();
}

Ct* CtModel::defaultCt()
{
    const QString defaultCtPath = APP_SETTINGS->defaultCtPath();
    if (!defaultCtPath.isEmpty()) {
        return forPath(defaultCtPath);
    }

    if (!m_cts.isEmpty()) {
        return m_cts.first();
    }

    return nullptr;
}

void CtModel::fetchAvailableReleases()
{
    if (!ctSourceMap().contains(m_ctSourceName)) {
        qCritical() << "No suitable source to obtain releases";
        return;
    }

    m_releaseUrlMap.clear();
    m_sortedReleasesList.clear();

    QNetworkRequest request(ctSourceMap().value(m_ctSourceName));
    request.setHeader(QNetworkRequest::UserAgentHeader, "kisel");

    QNetworkReply* reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onReleasesFetched(reply); });

    emit fetchReleasesStarted();
}

QString CtModel::deviceArchitecture()
{
    static QString arch = QSysInfo::currentCpuArchitecture().toLower();

    if (arch == "x86_64" || arch == "amd64") {
        return "x86_64";
    }

    if (arch == "arm64" || arch == "aarch64") {
        return "aarch64";
    }

    return "unknown";
}

bool CtModel::deviceHasV3Exstensions()
{
    static QString arch = deviceArchitecture();

    if (arch == "x86_64") {
        if (__builtin_cpu_supports("avx2") && __builtin_cpu_supports("fma") && __builtin_cpu_supports("bmi2")) {
            return true;
        }
    }

    return false;
}

QString CtModel::getBaseArchitectureNameFromAsset(const QString& assetName)
{
    if (assetName.contains("x86_64_v3", Qt::CaseInsensitive)) {
        return "x86_64_v3";
    }

    if (assetName.contains("aarch64", Qt::CaseInsensitive) || assetName.contains("arm64", Qt::CaseInsensitive)) {
        return "aarch64";
    }

    return "x86_64";
}

void CtModel::onReleasesFetched(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit fetchReleasesError(tr("Error loading releases: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    if (!jsonDoc.isArray()) {
        emit fetchReleasesError(tr("Invalid response format from Server API"));
        return;
    }

    parseReleasesArray(jsonDoc.array());

    emit fetchReleasesFinished();
}

void CtModel::parseReleasesArray(const QJsonArray& releasesArray)
{
    static const QString devArch = deviceArchitecture();
    static const bool isDevAarch64 = (devArch == "aarch64");
    static const bool isDevX86_64 = (devArch == "x86_64");
    static const bool supportsV3 = isDevX86_64 && deviceHasV3Exstensions();

    for (const auto& releaseVal : releasesArray) {
        const QJsonObject release = releaseVal.toObject();
        const QString tagName = release.value("tag_name").toString();
        const QJsonArray assetsArray = release.value("assets").toArray();

        const QUrl downloadUrl = findBestAssetUrl(assetsArray);

        if (!downloadUrl.isEmpty()) {
            m_releaseUrlMap.insert(tagName, downloadUrl);
            m_sortedReleasesList.append(tagName);
        }
    }
}

QUrl CtModel::findBestAssetUrl(const QJsonArray& assetsArray)
{
    static const QString devArch = deviceArchitecture();
    static const bool isDevAarch64 = (devArch == "aarch64");
    static const bool isDevX86_64 = (devArch == "x86_64");
    static const bool supportsV3 = isDevX86_64 && deviceHasV3Exstensions();

    QUrl fallbackUrl;

    for (const auto& assetVal : assetsArray) {
        const QJsonObject assetObj = assetVal.toObject();
        const QString assetName = assetObj.value("name").toString();

        const bool isTarball = assetName.endsWith(".tar.gz") || assetName.endsWith(".tar.xz");
        if (!isTarball) {
            continue;
        }

        const QString assetArch = getBaseArchitectureNameFromAsset(assetName);
        const QString assetUrl = assetObj.value("browser_download_url").toString();

        if (isDevAarch64 && assetArch == "aarch64") {
            return assetUrl;
        }

        if (isDevX86_64) {
            if (assetArch == "x86_64_v3" && supportsV3) {
                return assetUrl;
            }

            if (assetArch == "x86_64") {
                fallbackUrl = assetUrl;
            }
        }
    }

    return fallbackUrl;
}

void CtModel::installRelease(const QString& name, const QString& installDir)
{
    if (m_installationIsRunning) {
        return;
    }

    if (!m_releaseUrlMap.contains(name)) {
        emit installationError(tr("No suitable release name found: %1").arg(name));
        return;
    }

    if (!CTS_DIR_LIST.contains(installDir)) {
        emit installationError(tr("No suitable installation path specified: %1").arg(installDir));
        return;
    }

    QDir dir(installDir);
    if (!dir.exists()) {
        if (!dir.mkpath(installDir)) {
            emit installationError(tr("Cannot create directory: %1").arg(installDir));
            return;
        }
    }

    m_downloadableReleaseName = name;
    m_ctInstallDir = installDir;
    m_installationIsRunning = true;

    QNetworkRequest request(m_releaseUrlMap.value(name));
    request.setHeader(QNetworkRequest::UserAgentHeader, "kisel");

    m_downloadReply = m_networkManager.get(request);

    connect(m_downloadReply, &QNetworkReply::downloadProgress, this, &CtModel::downloadProgressChanged);
    connect(m_downloadReply, &QNetworkReply::finished, this, &CtModel::onDownloadFinished);

    emit downloadStarted();
}

void CtModel::onDownloadFinished()
{
    if (m_downloadReply->error() != QNetworkReply::NoError) {
        emit installationError(m_downloadReply->errorString());
        m_downloadReply->deleteLater();
        m_installationIsRunning = false;
        return;
    }

    auto* tempArchive = new QTemporaryFile(QDir::tempPath() + "/" + m_downloadableReleaseName + "_XXXXXX.tar.gz", this);

    if (tempArchive->open()) {
        tempArchive->setAutoRemove(false);
        QString tempFilePath = tempArchive->fileName();

        tempArchive->write(m_downloadReply->readAll());
        tempArchive->close();

        tempArchive->deleteLater();
        m_downloadReply->deleteLater();

        emit downloadFinished();

        extractCt(tempFilePath);
    } else {
        emit installationError(tr("Cannot create temporary file"));
        tempArchive->deleteLater();
        m_downloadReply->deleteLater();
        m_installationIsRunning = false;
    }
}

void CtModel::extractCt(const QString& archivePath)
{
    emit extractStarted();

    m_tarProcess = new QProcess(this);
    m_tarProcess->setWorkingDirectory(m_ctInstallDir);

    connect(m_tarProcess, &QProcess::finished, this, [this, archivePath](int exitCode) {
        m_tarProcess->deleteLater();

        if (QFile::exists(archivePath)) {
            QFile::remove(archivePath);
        }

        if (exitCode == 0) {
            emit extractFinished();
            m_installationIsRunning = false;
            refreshList();
        } else {
            emit installationError(tr("Archive extraction error: %1").arg(archivePath));
            m_installationIsRunning = false;
        }
    });

    m_tarProcess->start("tar", { "-xf", archivePath });
}