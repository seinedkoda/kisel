#include "prefix_list_widget.hpp"

#include <QDesktopServices>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"
#include "ui/prefix/new_prefix_dialog.hpp"
#include "ui/prefix/prefix_components_dialog.hpp"
#include "ui/prefix/prefix_settings_dialog.hpp"

using namespace kisel;

PrefixListWidget::PrefixListWidget(QWidget* parent)
    : QWidget(parent)
    , m_prefixListView(new QListView(this))
{
    setWindowTitle(tr("Kisel — Prefixes"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* layout = new QVBoxLayout(this);

    auto* listLabel = new QLabel(tr("<h3>Prefixes</h3>"), this);
    layout->addWidget(listLabel);

    m_prefixListView->setModel(PREFIX_MODEL);
    m_prefixListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_prefixListView, &QListView::customContextMenuRequested, this, &PrefixListWidget::onContextMenuRequested);
    layout->addWidget(m_prefixListView);

    auto* addNewButton = new QPushButton(QIcon::fromTheme("list-add"), tr("Add new"), this);
    connect(addNewButton, &QPushButton::clicked, this, [this]() {
        auto* dialog = new NewPrefixDialog(this);
        dialog->show();
    });
    layout->addWidget(addNewButton);
}

void PrefixListWidget::onContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = m_prefixListView->indexAt(pos);

    if (!index.isValid()) {
        return;
    }

    Prefix* prefix = PREFIX_MODEL->forIndex(index.row());

    QMenu menu(this);

    QAction* settingsAction = menu.addAction(QIcon::fromTheme("configure"), tr("Configure"));
    connect(settingsAction, &QAction::triggered, this, [this, prefix]() {
        auto* prefixSettingsDialog = new PrefixSettingsDialog(prefix, this);
        prefixSettingsDialog->exec();
    });

    QMenu* toolsMenu = menu.addMenu(QIcon::fromTheme("tools"), tr("Tools"));

    QAction* componentsAction = toolsMenu->addAction(QIcon::fromTheme("plugins"), tr("Install components"));
    connect(componentsAction, &QAction::triggered, this, [this, prefix]() {
        if (APP_SETTINGS->winetricksPath().isEmpty()) {
            QMessageBox::critical(this, tr("Opening error"), tr("\"winetricks\" not found! Please install this package to open this window"));
            return;
        }

        auto* prefixComponentsDialog = new PrefixComponentsDialog(prefix, this);
        prefixComponentsDialog->exec();
    });

    QAction* winecfgAction = toolsMenu->addAction(QIcon::fromTheme("wine-symbolic"), tr("Wine settings"));
    connect(winecfgAction, &QAction::triggered, this, [prefix]() { RUN_MANAGER->runWineCfg(prefix); });

    QAction* explorerAction = toolsMenu->addAction(QIcon::fromTheme("document-open-folder"), tr("Explorer"));
    connect(explorerAction, &QAction::triggered, this, [prefix]() { RUN_MANAGER->runExplorer(prefix); });

    QAction* regeditAction = toolsMenu->addAction(QIcon::fromTheme("view-list-text"), tr("Registry"));
    connect(regeditAction, &QAction::triggered, this, [prefix]() { RUN_MANAGER->runRegedit(prefix); });

    QAction* uninstallerAction = toolsMenu->addAction(QIcon::fromTheme("entry-delete"), tr("Remove programs"));
    connect(uninstallerAction, &QAction::triggered, this, [prefix]() { RUN_MANAGER->runUninstaller(prefix); });

    QAction* openAction = menu.addAction(QIcon::fromTheme("document-open-folder"), tr("Open in files"));
    connect(openAction, &QAction::triggered, this, [prefix]() { QDesktopServices::openUrl(QUrl::fromLocalFile(prefix->path())); });

    menu.addSeparator();

    QAction* removeAction = menu.addAction(QIcon::fromTheme("entry-delete"), tr("Delete"));
    connect(removeAction, &QAction::triggered, this, [this, prefix, index]() {
        if (QMessageBox::question(this, tr("Confirm"), tr("Remove the \"%1\" prefix?").arg(prefix->name())) == QMessageBox::Yes) {
            PREFIX_MODEL->removeRow(index.row());
        }
    });

    menu.exec(QCursor::pos());
}
