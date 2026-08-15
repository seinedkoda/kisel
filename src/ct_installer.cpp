#include "ct_installer.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QNetworkReply>
#include <QTemporaryFile>

#include "app_settings.hpp"
#include "ct_model.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

CtInstallProcess::CtInstallProcess(Ct* ct, QNetworkReply* reply, QObject* parent)
    : QObject(parent)
    , ct(ct)
    , reply(reply)
    , tarProcess(new QProcess(this))
{
    connect(reply, &QNetworkReply::downloadProgress, this, [ct](qint64 bytesReceived, qint64 bytesTotal) {
        CT_MODEL->setCtDownloadProgress(ct, bytesReceived, bytesTotal);
    });
    connect(reply, &QNetworkReply::finished, this, &CtInstallProcess::onDownloadFinished);
}

void CtInstallProcess::onDownloadFinished()
{
    if (reply->error() != QNetworkReply::NoError) {
        emit finished(false, reply->errorString());
        return;
    }

    extractCt();
}

void CtInstallProcess::extractCt()
{
    CT_MODEL->setCtStatus(ct, Ct::Unpacking);

    auto* tempArchive = new QTemporaryFile(QDir::tempPath() % "/"_L1 % ct->name() % "_XXXXXX.tar.gz"_L1, this);
    if (!tempArchive->open()) {
        tempArchive->deleteLater();

        emit finished(false, tr("Cannot create temporary file"));
        return;
    }

    const QString tempArchivePath = tempArchive->fileName();
    tempArchive->setAutoRemove(false);

    tempArchive->write(reply->readAll());

    tempArchive->close();
    tempArchive->deleteLater();

    if (!ct->exists()) {
        if (!ct->dir().mkpath(".")) {
            emit finished(false, tr("Cannot create directory: %1").arg(ct->path()));
            return;
        }
    }

    tarProcess->setWorkingDirectory(ct->path());

    connect(tarProcess, &QProcess::finished, this, [this, tempArchivePath](int exitCode) {
        if (QFile::exists(tempArchivePath)) {
            QFile::remove(tempArchivePath);
        }

        if (exitCode == 0) {
            emit finished(true);
        } else {
            emit finished(false, tr("Archive extraction error: %1").arg(tarProcess->errorString()));
        }
    });

    // Extract without subfolder
    tarProcess->start("tar"_L1, { "-xf"_L1, tempArchivePath, "--strip-components=1" });
}

CtInstallProcess::~CtInstallProcess()
{
    if (reply) {
        reply->abort();
        reply->deleteLater();
    }

    tarProcess->terminate();
    if (!tarProcess->waitForFinished()) {
        tarProcess->kill();
    }
}

CtInstaller::CtInstaller(QObject* parent)
    : QObject(parent)
{
}

CtInstaller* CtInstaller::instance()
{
    static CtInstaller instance;
    return &instance;
}

QString CtInstaller::defaultCtSource()
{
    return "Proton-GE (Github)"_L1;
}

const QMap<QString, QUrl>& CtInstaller::ctSourceMap()
{
    static QMap<QString, QUrl> map {
        { "Proton-GE (Github)"_L1, QUrl("https://api.github.com/repos/GloriousEggroll/proton-ge-custom/releases?per_page=10"_L1) },
        { "Proton-CachyOS (Github)"_L1, QUrl("https://api.github.com/repos/CachyOS/proton-cachyos/releases?per_page=10"_L1) }
    };
    return map;
}

void CtInstaller::fetchReleases(const QString& sourceName, QObject* requester)
{
    if (!ctSourceMap().contains(sourceName)) {
        return;
    }

    QNetworkRequest request(ctSourceMap().value(sourceName));
    request.setHeader(QNetworkRequest::UserAgentHeader, "kisel"_L1);
    QNetworkReply* reply = m_networkManager.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, requester]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit releasesLoaded(requester, { }, false, reply->errorString());
            reply->deleteLater();
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();

        if (!jsonDoc.isArray()) {
            emit releasesLoaded(requester, { }, false, tr("Invalid response format from Server API"));
            return;
        }

        QMap<QString, QUrl> releaseMap = parseReleasesArray(jsonDoc.array());
        emit releasesLoaded(requester, releaseMap, true, { });
    });
}

void CtInstaller::addToInstallation(const QString& name, const QUrl& url, const QString& installRootPath)
{
    if (name.isEmpty() || url.isEmpty() || installRootPath.isEmpty()) {
        return;
    }

    if (!CTS_DIR_LIST.contains(installRootPath)) {
        emit installationError(tr("No suitable installation path specified: %1").arg(installRootPath));
        return;
    }

    QDir installRootDir(installRootPath);
    if (!installRootDir.exists()) {
        if (!installRootDir.mkpath(".")) {
            emit installationError(tr("Cannot create directory: %1").arg(installRootPath));
            return;
        }
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "kisel"_L1);
    QNetworkReply* reply = m_networkManager.get(request);

    Ct* ct = CT_MODEL->add(installRootDir.filePath(name));
    CT_MODEL->setCtStatus(ct, Ct::Downloading);

    auto* installItem = new CtInstallProcess(ct, reply, this);
    m_installList.append(installItem);

    connect(installItem, &CtInstallProcess::finished, this, [this, installItem, ct](bool success, const QString& errorText) {
        installItem->deleteLater();
        m_installList.removeOne(installItem);
        if (success) {
            CT_MODEL->setCtStatus(ct, Ct::Installed);
        } else {
            CT_MODEL->removeRow(CT_MODEL->ctIndex(ct));
            emit installationError(errorText);
        }
    });
}

void CtInstaller::cancelInstallation(Ct* ct)
{
    for (auto* item : std::as_const(m_installList)) {
        if (ct == item->ct) {
            item->deleteLater();
            m_installList.removeOne(item);
            return;
        }
    }
}

QString CtInstaller::deviceArchitecture()
{
    static QString arch = QSysInfo::currentCpuArchitecture().toLower();

    if (arch == "x86_64"_L1 || arch == "amd64"_L1) {
        return "x86_64";
    }

    if (arch == "arm64"_L1 || arch == "aarch64"_L1) {
        return "aarch64"_L1;
    }

    return "unknown"_L1;
}

bool CtInstaller::deviceHasV3Exstensions()
{
    static QString arch = deviceArchitecture();

    if (arch == "x86_64"_L1) {
        if (__builtin_cpu_supports("avx2") && __builtin_cpu_supports("fma") && __builtin_cpu_supports("bmi2")) {
            return true;
        }
    }

    return false;
}

QString CtInstaller::getBaseArchitectureNameFromAsset(const QString& assetName)
{
    if (assetName.contains("x86_64_v3"_L1, Qt::CaseInsensitive)) {
        return "x86_64_v3"_L1;
    }

    if (assetName.contains("aarch64"_L1, Qt::CaseInsensitive) || assetName.contains("arm64"_L1, Qt::CaseInsensitive)) {
        return "aarch64"_L1;
    }

    return "x86_64"_L1;
}

QMap<QString, QUrl> CtInstaller::parseReleasesArray(const QJsonArray& releasesArray)
{
    QMap<QString, QUrl> releaseUrlMap;
    static const QString devArch = deviceArchitecture();
    static const bool isDevAarch64 = (devArch == "aarch64"_L1);
    static const bool isDevX86_64 = (devArch == "x86_64"_L1);
    static const bool supportsV3 = isDevX86_64 && deviceHasV3Exstensions();

    for (const auto& releaseVal : releasesArray) {
        const QJsonObject release = releaseVal.toObject();
        const QString tagName = release.value("tag_name"_L1).toString();
        const QJsonArray assetsArray = release.value("assets"_L1).toArray();

        const QUrl downloadUrl = findBestAssetUrl(assetsArray);

        if (!downloadUrl.isEmpty()) {
            releaseUrlMap.insert(tagName, downloadUrl);
        }
    }

    return releaseUrlMap;
}

QUrl CtInstaller::findBestAssetUrl(const QJsonArray& assetsArray)
{
    static const QString devArch = deviceArchitecture();
    static const bool isDevAarch64 = (devArch == "aarch64"_L1);
    static const bool isDevX86_64 = (devArch == "x86_64"_L1);
    static const bool supportsV3 = isDevX86_64 && deviceHasV3Exstensions();

    QUrl fallbackUrl;

    for (const auto& assetVal : assetsArray) {
        const QJsonObject assetObj = assetVal.toObject();
        const QString assetName = assetObj.value("name"_L1).toString();

        const bool isTarball = assetName.endsWith(".tar.gz"_L1) || assetName.endsWith(".tar.xz"_L1);
        if (!isTarball) {
            continue;
        }

        const QString assetArch = getBaseArchitectureNameFromAsset(assetName);
        const QString assetUrl = assetObj.value("browser_download_url"_L1).toString();

        if (isDevAarch64 && assetArch == "aarch64"_L1) {
            return assetUrl;
        }

        if (isDevX86_64) {
            if (assetArch == "x86_64_v3"_L1 && supportsV3) {
                return assetUrl;
            }

            if (assetArch == "x86_64"_L1) {
                fallbackUrl = assetUrl;
            }
        }
    }

    return fallbackUrl;
}
