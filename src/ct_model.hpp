#pragma once

#include <QAbstractListModel>
#include <QDir>
#include <QNetworkReply>
#include <QProcess>

#include "ct.hpp"

namespace kisel {
#define CT_MODEL CtModel::instance()

class CtModel : public QAbstractListModel { // NOLINT(cppcoreguidelines-virtual-class-destructor)
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(CtModel)

public:
    static CtModel* instance();

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] Ct* forIndex(int index) const;
    [[nodiscard]] Ct* forPath(QStringView path) const;
    [[nodiscard]] QStringList availableReleasesList() const;
    [[nodiscard]] QString ctSourceName() const;
    [[nodiscard]] bool installationIsRunning() const;
    void refreshList();
    void add(const QString& path);
    void remove(const QModelIndex& itemIndex);
    void setCtSourceFromName(const QString& name);
    void fetchAvailableReleases();
    void installRelease(const QString& name, const QString& installDir);
    void cancelInstallation();
    Ct* defaultCt();
    static const QMap<QString, QUrl>& ctSourceMap();

    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole
    };
signals:
    void fetchReleasesStarted();
    void fetchReleasesFinished();
    void fetchReleasesError(const QString& errorText);
    void installationError(const QString& errorText);
    void installationCanceled();
    void downloadStarted();
    void downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void extractStarted();
    void extractFinished();
private slots:
    void onReleasesFetched(QNetworkReply* reply);
    void onDownloadFinished();

private:
    explicit CtModel(QObject* parent = nullptr);
    ~CtModel() override;

    static QString deviceArchitecture();
    static bool deviceHasV3Exstensions();
    static QString getBaseArchitectureNameFromAsset(const QString& assetName);
    void parseReleasesArray(const QJsonArray& releasesArray);
    static QUrl findBestAssetUrl(const QJsonArray& assetsArray);
    bool containsPath(QStringView path);
    void extractCt(const QString& archivePath);

    QNetworkAccessManager m_networkManager;
    QMap<QString, QUrl> m_releaseUrlMap;
    QList<Ct*> m_cts;
    QStringList m_sortedReleasesList;
    QNetworkReply* m_downloadReply { };
    QProcess* m_tarProcess { };
    QString m_ctSourceName;
    QString m_ctInstallDir;
    QString m_downloadableReleaseName;
    bool m_installationIsRunning = false;
};
}
