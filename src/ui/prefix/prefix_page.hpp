#pragma once

#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

namespace kisel {
class PrefixPage : public QWidget {
    Q_OBJECT

public:
    explicit PrefixPage(QWidget* parent = nullptr);

private slots:
    void onPrefixesDirSelectClicked();
    void onPrefixesDirResetClicked();

private:
    QLineEdit* m_sharedPrefixesDirLineEdit;
    QToolButton* m_sharedPrefixesDirSelectButton;
    QToolButton* m_sharedPrefixesDirResetButton;
};
}