#include "app_settings_window.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>

#include "ui/settingspages/app_general_settings_page.hpp"
#include "ui/settingspages/app_platform_settings_page.hpp"
#include "ui/settingspages/app_prefixes_settings_page.hpp"
#include "ui/settingspages/compatibility_settings_page.hpp"
#include "ui/settingspages/services_settings_page.hpp"
#include "ui/shortcuts/shortcuts_list_widget.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

AppSettingsWindow::AppSettingsWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Kisel — Settings"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumWidth(400);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QVBoxLayout(centralWidget);

    auto* titleLabel = new QLabel(tr("<h3>Global settings</h3>"));
    layout->addWidget(titleLabel);

    auto* helpLabel = new QLabel(tr("<i>Hover over the option to learn more</i>"), this);
    helpLabel->setWordWrap(true);
    layout->addWidget(helpLabel);

    auto* contentWidget = new QWidget(this);
    layout->addWidget(contentWidget);

    auto* contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    auto* pageListView = new QListWidget(this);
    contentLayout->addWidget(pageListView, Qt::AlignLeft);

    auto* stackedPages = new QStackedWidget(this);
    connect(pageListView, &QListWidget::currentRowChanged, this, [stackedPages](int index) {
        stackedPages->setCurrentIndex(index);
    });
    contentLayout->addWidget(stackedPages);

    auto* generalPage = new AppSettingsGeneralPage(this);
    new QListWidgetItem(QIcon::fromTheme("user-home-symbolic"), tr("General"), pageListView);
    stackedPages->addWidget(generalPage);

    auto* prefixesPage = new AppPrefixesSettingsPage(APP_SETTINGS, this);
    new QListWidgetItem(QIcon::fromTheme("drive-harddisk-symbolic"), tr("Prefixes"), pageListView);
    stackedPages->addWidget(prefixesPage);

    auto* shortcutPage = new ShortcutsListWidget(this);
    shortcutPage->layout()->setContentsMargins(0, 0, 0, 0);
    new QListWidgetItem(QIcon::fromTheme("link"), tr("Shortcuts"), pageListView);
    stackedPages->addWidget(shortcutPage);

    auto* ctPage = new CompatibilitySettingsPage(APP_SETTINGS, this);
    new QListWidgetItem(QIcon::fromTheme("tools-wizard"), tr("Compatibility"), pageListView);
    stackedPages->addWidget(ctPage);

    auto* platformPage = new AppPlatformSettingsPage(APP_SETTINGS, this);
    platformPage->layout()->setContentsMargins(0, 0, 0, 0);
    new QListWidgetItem(QIcon::fromTheme("computer-symbolic"), tr("Platform"), pageListView);
    stackedPages->addWidget(platformPage);

    auto* servicesPage = new ServicesSettingsPage(APP_SETTINGS, this);
    new QListWidgetItem(QIcon::fromTheme("flag"), tr("Services"), pageListView);
    stackedPages->addWidget(servicesPage);

    pageListView->setCurrentRow(0);
    pageListView->setFixedWidth(pageListView->sizeHintForColumn(0) + 6);
    resize(550, height());
}
