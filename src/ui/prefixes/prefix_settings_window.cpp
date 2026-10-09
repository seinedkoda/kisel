#include "prefix_settings_window.hpp"

#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "ui/settingspages/compatibility_settings_page.hpp"
#include "ui/settingspages/platform_settings_page.hpp"
#include "ui/settingspages/services_settings_page.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixSettingsWindow::PrefixSettingsWindow(Prefix* prefix, QWidget* parent)
    : QWidget(parent)
    , m_settings(prefix->settings())
{
    setWindowTitle(tr("Kisel — Prefix Settings"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* titleLabel = new QLabel(tr("<h3>Settings for the \"%1\" prefix</h3>").arg(prefix->name()));
    layout->addWidget(titleLabel);

    auto* helpLabel = new QLabel(tr("<i>Hover over the option to learn more</i>"), this);
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

    auto* platformPage = new PlatformSettingsPage(m_settings, this);
    platformPage->layout()->setContentsMargins(0, 0, 0, 0);
    new QListWidgetItem(QIcon::fromTheme("computer-symbolic"), tr("Platform"), pageListView);
    stackedPages->addWidget(platformPage);

    auto* compatibilityPage = new CompatibilitySettingsPage(m_settings, this);
    new QListWidgetItem(QIcon::fromTheme("tools-wizard"), tr("Compatibility"), pageListView);
    stackedPages->addWidget(compatibilityPage);

    auto* servicesPage = new ServicesSettingsPage(m_settings, this);
    new QListWidgetItem(QIcon::fromTheme("flag"), tr("Services"), pageListView);
    stackedPages->addWidget(servicesPage);

    pageListView->setCurrentRow(0);
    pageListView->setFixedWidth(pageListView->sizeHintForColumn(0) + 6);
}
