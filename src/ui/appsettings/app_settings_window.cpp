#include "app_settings_window.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStyleFactory>

#include "core/appsettings/app_settings.hpp"
#include "ui/compatibilitytools/ct_page.hpp"
#include "ui/prefix/prefix_page.hpp"
#include "ui/shortcuts/shortcuts_list_widget.hpp"

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

    auto* helpLabel = new QLabel(tr("<i>To configure the executable launch settings in detail, "
                                    "go to the context menu of the selected prefix</i>"), this);
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

    auto* generalPage = new QWidget(this);
    new QListWidgetItem(QIcon::fromTheme("user-home-symbolic"), tr("General"), pageListView);
    stackedPages->addWidget(generalPage);

    auto* generalPageLayout = new QVBoxLayout(generalPage);
    generalPageLayout->setAlignment(Qt::AlignTop);

    auto* languageLabel = new QLabel(tr("Language"), this);
    generalPageLayout->addWidget(languageLabel);

    auto* languageComboBox = new QComboBox(this);
    languageComboBox->addItems(APP_SETTINGS->languagesList());
    languageComboBox->setCurrentText(APP_SETTINGS->language());
    connect(languageComboBox, &QComboBox::currentTextChanged, this, [](const QString& languageName) {
        APP_SETTINGS->setLanguage(languageName);
    });
    generalPageLayout->addWidget(languageComboBox);

    auto* styleLabel = new QLabel(tr("Style"), this);
    generalPageLayout->addWidget(styleLabel);

    auto* styleComboBox = new QComboBox(this);
    static QStringList styles = QStyleFactory::keys();
    styleComboBox->addItems(styles);
    const QString savedStyle = APP_SETTINGS->styleName();
    if (styles.contains(savedStyle)) {
        styleComboBox->setCurrentText(savedStyle);
    } else {
        styleComboBox->setCurrentText(QStringLiteral("Fusion"));
    }
    connect(styleComboBox, &QComboBox::currentTextChanged, this, [](const QString& styleName) { APP_SETTINGS->setStyleName(styleName); });
    generalPageLayout->addWidget(styleComboBox);

    auto* loggingCheckBox = new QCheckBox(tr("Logging"), this);
    loggingCheckBox->setChecked(APP_SETTINGS->loggingEnabled());
    connect(loggingCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setLoggingEnabled(checked);
    });
    generalPageLayout->addWidget(loggingCheckBox);

    auto* prefixPage = new PrefixPage(this);
    new QListWidgetItem(QIcon::fromTheme("drive-symbolic"), tr("Prefixes"), pageListView);
    stackedPages->addWidget(prefixPage);

    auto* ctPage = new CtPage(this);
    new QListWidgetItem(QIcon::fromTheme("tools-wizard"), tr("Compatibility"), pageListView);
    stackedPages->addWidget(ctPage);

    auto* shortcutPage = new ShortcutsListWidget(this);
    new QListWidgetItem(QIcon::fromTheme("link"), tr("Shortcuts"), pageListView);
    stackedPages->addWidget(shortcutPage);

    pageListView->setFixedWidth(pageListView->sizeHintForColumn(0) + 6);
    resize(550, height());
}
