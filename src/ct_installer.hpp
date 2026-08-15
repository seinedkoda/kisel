#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QPointer>
#include <QProcess>

#include "ct.hpp"

namespace kisel {
#define CT_INSTALLER CtInstaller::instance()

class CtInstallProcess : public QObject { // NOLINT(cppcoreguidelines-virtual-class-destructor)
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(CtInstallProcess)

public:
    explicit CtInstallProcess(Ct* ct, QNetworkReply* reply, QObject* parent = nullptr);
    ~CtInstallProcess() override;

    Ct* ct;
    QPointer<QNetworkReply> reply;
    QProcess* tarProcess;

signals:
    void finished(bool success, const QString& errorText = "");

private slots:
    void onDownloadFinished();

private:
    void extractCt();
};

class CtInstaller : public QObject {
    Q_OBJECT

public:
    static CtInstaller* instance();

    static QString defaultCtSource();
    static const QMap<QString, QUrl>& ctSourceMap();
    void fetchReleases(const QString& sourceName, QObject* requester);
    void addToInstallation(const QString& name, const QUrl& url, const QString& installDir);
    void cancelInstallation(Ct* ct);

signals:
    void releasesLoaded(QObject* requester, const QMap<QString, QUrl>& releaseMap, bool success, const QString& errorText);
    void installationError(const QString& errorText);
    void newInstalled();

private:
    explicit CtInstaller(QObject* parent = nullptr);

    static QString deviceArchitecture();
    static bool deviceHasV3Exstensions();
    static QString getBaseArchitectureNameFromAsset(const QString& assetName);
    static QMap<QString, QUrl> parseReleasesArray(const QJsonArray& releasesArray);
    static QUrl findBestAssetUrl(const QJsonArray& assetsArray);

    QList<CtInstallProcess*> m_installList;
    QNetworkAccessManager m_networkManager;
};
}