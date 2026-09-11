#pragma once

#include <QComboBox>
#include <QDialog>
#include <QToolButton>

namespace kisel {
class NewCtDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewCtDialog(QWidget* parent = nullptr);

private slots:
    void fetchAvailableReleases();
    void onReleasesLoaded(QObject* requester, const QMap<QString, QUrl>& releaseMap, bool success, const QString& errorText);
    void onInstallClicked();

private:
    QComboBox* m_ctSourceComboBox;
    QComboBox* m_releasesComboBox;
    QToolButton* m_refreshReleasesButton;
    QComboBox* m_installationLocationsComboBox;
    QPushButton* m_addToInstallationButton;
};
}