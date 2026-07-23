#pragma once

#include <QComboBox>
#include <QListView>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QToolButton>

namespace kisel {
class CtWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit CtWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
private slots:
    void onFetchReleasesStarted();
    void onFetchReleasesFinished();
    void onFetchReleasesError(const QString& errorText);
    void onDownloadStarted();
    void onDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);
    void onExtractionStarted();
    void onExtractionFinished();
    void onInstallationError(const QString& errorText);
    void onInstallationCanceled();
    void onContextMenuRequested(const QPoint& pos);

private:
    QListView* m_ctListView;
    QComboBox* m_ctSourceComboBox;
    QComboBox* m_releasesComboBox;
    QToolButton* m_refreshReleasesButton;
    QComboBox* m_installationLocationsComboBox;
    QPushButton* m_installCancelButton;
    QProgressBar* m_progressBar;
    QMenu* m_listViewContextMenu;
    QAction* m_openAction;
    QAction* m_deleteAction;

    void resetInstallationWidgetsState();
};
}