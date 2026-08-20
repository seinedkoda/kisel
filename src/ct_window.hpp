#pragma once

#include <QComboBox>
#include <QDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QToolButton>

namespace kisel {
class CtWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit CtWindow(QWidget* parent = nullptr);

private slots:
    void openAddNewCtDialog();
    void onInstallationError(const QString& errorText);
    void onContextMenuRequested(const QPoint& pos);

private:
    QTableView* m_ctTableView;
};

class ProgressBarDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class AddNewCtDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddNewCtDialog(QWidget* parent = nullptr);

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