#include "prefix_window.hpp"

#include <QDesktopServices>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

#include "prefix_model.hpp"

using namespace kisel;

AddNewPrefixDialog::AddNewPrefixDialog(QWidget* parent)
    : QDialog(parent)
    , m_nameInput(new QLineEdit(this))
{
    setWindowTitle(tr("Add new prefix"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(400);

    auto* layout = new QVBoxLayout(this);

    auto* nameLabel = new QLabel(tr("Enter the prefix name"), this);
    layout->addWidget(nameLabel);

    m_nameInput->setPlaceholderText(tr("Name"));
    layout->addWidget(m_nameInput);

    m_saveButton = new QPushButton(tr("Save"), this);
    m_saveButton->setEnabled(false);
    layout->addWidget(m_saveButton);

    connect(m_nameInput, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_saveButton->setEnabled(PREFIX_MODEL->isValidPrefixName(text));
    });

    connect(m_saveButton, &QPushButton::clicked, this, [this]() {
        Prefix* prefix = PREFIX_MODEL->add(m_nameInput->text());
        prefix->makePath();
        close();
    });
}

PrefixWindow::PrefixWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_prefixListView(new QListView(this))
    , m_listViewContextMenu(new QMenu(this))
{
    setWindowTitle(tr("Kisel — Prefixes"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* centralWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    auto* listLabel = new QLabel(tr("Detected prefixes:"), this);
    layout->addWidget(listLabel);

    m_prefixListView->setModel(PREFIX_MODEL);
    m_prefixListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_prefixListView, &QListView::customContextMenuRequested, this, &PrefixWindow::onContextMenuRequested);
    layout->addWidget(m_prefixListView);

    m_openAction = m_listViewContextMenu->addAction(QIcon::fromTheme("document-open-folder"), tr("Open in files"));
    m_deleteAction = m_listViewContextMenu->addAction(QIcon::fromTheme("remove"), tr("Delete"));

    auto* addNewButton = new QPushButton(tr("Add new"), this);
    addNewButton->setIcon(QIcon::fromTheme("add"));
    connect(addNewButton, &QPushButton::clicked, this, [this]() {
        auto* dialog = new AddNewPrefixDialog(this);
        dialog->show();
    });
    layout->addWidget(addNewButton);
}

void PrefixWindow::onContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = m_prefixListView->indexAt(pos);

    if (!index.isValid()) {
        return;
    }

    QAction* selectedAction = m_listViewContextMenu->exec(pos);

    if (selectedAction == m_openAction) {
        QString prefixPath = PREFIX_MODEL->data(index, PrefixModel::PathRole).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(prefixPath));
    } else if (selectedAction == m_deleteAction) {
        QString prefixName = index.data().toString();
        if (QMessageBox::question(this, tr("Confirm"), tr("Remove the \"%1\" prefix?").arg(prefixName)) == QMessageBox::Yes) {
            PREFIX_MODEL->removeRow(index.row());
        }
    }
}
