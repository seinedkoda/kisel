#include "prefix_window.hpp"

#include <QDesktopServices>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

#include "app_settings.hpp"
#include "prefix_components_dialog.hpp"
#include "prefix_model.hpp"
#include "prefix_settings_dialog.hpp"
#include "process_manager.hpp"

using namespace kisel;

AddNewPrefixDialog::AddNewPrefixDialog(QWidget* parent)
    : QDialog(parent)
    , m_nameInput(new QLineEdit(this))
    , m_saveButton(new QPushButton(QIcon::fromTheme("document-save"), tr("Save"), this))
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

    m_saveButton->setEnabled(false);

    auto* closeButton = new QPushButton(QIcon::fromTheme("window-close"), tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &AddNewPrefixDialog::close);

    auto* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(m_saveButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);
    layout->addWidget(buttonBox);

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

    auto* addNewButton = new QPushButton(QIcon::fromTheme("list-add"), tr("Add new"), this);
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

    Prefix* prefix = PREFIX_MODEL->forIndex(index.row());

    auto* menu = new QMenu(this);

    QAction* settingsAction = menu->addAction(QIcon::fromTheme("configure"), tr("Configure"));
    connect(settingsAction, &QAction::triggered, this, [this, prefix]() {
        auto* prefixSettingsDialog = new PrefixSettingsDialog(prefix, this);
        prefixSettingsDialog->exec();
    });

    QMenu* toolsMenu = menu->addMenu(QIcon::fromTheme("tools"), tr("Tools"));

    QAction* componentsAction = toolsMenu->addAction(QIcon::fromTheme("plugins"), tr("Install components"));
    connect(componentsAction, &QAction::triggered, this, [this, prefix]() {
        if (APP_SETTINGS->winetricksPath().isEmpty()) {
            QMessageBox::critical(this, tr("Opening error"), tr("\"winetricks\" not found! Please install this package to open this window"));
            return;
        }

        auto* prefixComponentsDialog = new PrefixComponentsDialog(*prefix, this);
        prefixComponentsDialog->exec();
    });

    QAction* winecfgAction = toolsMenu->addAction(QIcon::fromTheme("wine-symbolic"), tr("Wine settings"));
    connect(winecfgAction, &QAction::triggered, this, [prefix]() { PROCESS_MANAGER->runWineCfg(prefix); });

    QAction* explorerAction = toolsMenu->addAction(QIcon::fromTheme("document-open-folder"), tr("Explorer"));
    connect(explorerAction, &QAction::triggered, this, [prefix]() { PROCESS_MANAGER->runExplorer(prefix); });

    QAction* regeditAction = toolsMenu->addAction(QIcon::fromTheme("view-list-text"), tr("Registry"));
    connect(regeditAction, &QAction::triggered, this, [prefix]() { PROCESS_MANAGER->runRegedit(prefix); });

    QAction* uninstallerAction = toolsMenu->addAction(QIcon::fromTheme("entry-delete"), tr("Remove programs"));
    connect(uninstallerAction, &QAction::triggered, this, [prefix]() { PROCESS_MANAGER->runUninstaller(prefix); });

    QAction* openAction = menu->addAction(QIcon::fromTheme("document-open-folder"), tr("Open in files"));
    connect(openAction, &QAction::triggered, this, [prefix]() { QDesktopServices::openUrl(QUrl::fromLocalFile(prefix->path())); });

    menu->addSeparator();

    QAction* removeAction = menu->addAction(QIcon::fromTheme("list-remove"), tr("Delete"));
    connect(removeAction, &QAction::triggered, this, [this, prefix, index]() {
        if (QMessageBox::question(this, tr("Confirm"), tr("Remove the \"%1\" prefix?").arg(prefix->name())) == QMessageBox::Yes) {
            PREFIX_MODEL->removeRow(index.row());
        }
    });

    menu->exec(QCursor::pos());
}
