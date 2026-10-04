#include "main_window.hpp"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"
#include "core/compatibilitytools/ct_model.hpp"
#include "core/prefix/prefix.hpp"
#include "ui/aboutapp/about_app_dialog.hpp"
#include "ui/appsettings/app_settings_window.hpp"
#include "ui/compatibilitytools/ct_list_widget.hpp"
#include "ui/prefix/prefix_components_dialog.hpp"
#include "ui/prefix/prefix_list_widget.hpp"
#include "ui/prefix/prefix_settings_dialog.hpp"
#include "ui/shortcuts/edit_shortcuts_dialog.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

MainWindow::MainWindow(const QString& exePath)
    : QMainWindow(nullptr)
    , m_runConfig(RUN_MANAGER->config())
    , m_exeIconLabel(new QLabel(this))
    , m_exeNameLabel(new QLabel(tr("The program is not selected"), this))
    , m_runStopAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("Run"), this))
    , m_runStopButton(new QToolButton(this))
    , m_exeSelectionButton(new QToolButton(this))
    , m_prefixTypeComboBox(new QComboBox(this))
    , m_prefixComboBox(new QComboBox(this))
    , m_prefixMenuButton(new QToolButton(this))
    , m_ctComboBox(new QComboBox(this))
    , m_ctWindowButton(new QToolButton(this))
{
    m_runConfig->setExecutablePath(exePath);

    setWindowTitle(tr("Kisel"));
    setWindowIcon(QIcon(":/icons/kisel-256x256.png"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QVBoxLayout(centralWidget);

    auto* exeWidget = new QWidget(this);
    layout->addWidget(exeWidget);

    auto* exeLayout = new QGridLayout(exeWidget);
    exeLayout->setAlignment(Qt::AlignLeft);

    m_exeIconLabel->setFixedSize(m_exeIconSize);
    m_exeIconLabel->setScaledContents(true);
    m_exeIconLabel->setPixmap(m_unknownExePixmap);
    exeLayout->addWidget(m_exeIconLabel, 0, 0, 2, 1);

    auto* rightExeIconLine = new QFrame(this);
    rightExeIconLine->setFrameShape(QFrame::VLine);
    exeLayout->addWidget(rightExeIconLine, 0, 1, 2, 1);

    exeLayout->addWidget(m_exeNameLabel, 0, 2);

    auto* exeActionsWidget = new QWidget(this);
    exeLayout->addWidget(exeActionsWidget, 1, 2);

    auto* exeActionsLayout = new QHBoxLayout(exeActionsWidget);
    exeActionsLayout->setAlignment(Qt::AlignLeft);
    exeActionsLayout->setContentsMargins(0, 0, 0, 0);

    auto* exeMenu = new QMenu(this);

    auto* editShortcutsAction = exeMenu->addAction(QIcon::fromTheme("link"), tr("Shortcuts"));
    connect(editShortcutsAction, &QAction::triggered, this, &MainWindow::onEditShortcutsTriggered);

    auto* clearExeAction = exeMenu->addAction(QIcon::fromTheme("edit-clear"), tr("Clear"));
    connect(clearExeAction, &QAction::triggered, this, [this]() { setExecutablePath(""); });

    connect(m_runStopAction, &QAction::triggered, this, &MainWindow::onRunStopTriggered);

    m_runStopButton->setDefaultAction(m_runStopAction);
    m_runStopButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_runStopButton->setMenu(exeMenu);
    m_runStopButton->setPopupMode(QToolButton::MenuButtonPopup);
    exeActionsLayout->addWidget(m_runStopButton, Qt::AlignLeft);

    m_exeSelectionButton->setToolTip(tr("Select executable file"));
    m_exeSelectionButton->setIcon(QIcon::fromTheme("document-open"));
    connect(m_exeSelectionButton, &QToolButton::clicked, this, &MainWindow::onExeSelectionClicked);
    exeActionsLayout->addWidget(m_exeSelectionButton, Qt::AlignLeft);

    auto* environmentBox = new QGroupBox(tr("Environment"), this);
    environmentBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(environmentBox);
    auto* environmentBoxLayout = new QGridLayout(environmentBox);
    environmentBoxLayout->setAlignment(Qt::AlignTop);

    auto* prefixLabel = new QLabel(tr("Prefix"), this);
    environmentBoxLayout->addWidget(prefixLabel, 0, 0);

    auto* prefixTypeWidget = new QWidget(this);
    environmentBoxLayout->addWidget(prefixTypeWidget, 1, 0);

    auto* prefixTypeLayout = new QHBoxLayout(prefixTypeWidget);
    prefixTypeLayout->setContentsMargins(0, 0, 0, 0);

    auto* prefixTypeLabel = new QLabel(tr("Type"), this);
    prefixTypeLayout->addWidget(prefixTypeLabel);

    m_prefixTypeComboBox->addItems({ tr("Shared"), tr("Individual"), tr("Portable") });
    m_prefixTypeComboBox->setCurrentIndex(APP_SETTINGS->prefixType());
    prefixTypeLayout->addWidget(m_prefixTypeComboBox, 1);

    m_prefixComboBox->setModel(PREFIX_MODEL);
    environmentBoxLayout->addWidget(m_prefixComboBox, 2, 0);

    auto* prefixMenu = new QMenu(m_prefixMenuButton);

    m_prefixSettingsAction = prefixMenu->addAction(QIcon::fromTheme("configure"), tr("Configure"));
    connect(m_prefixSettingsAction, &QAction::triggered, this, [this]() {
        auto* prefixSettingsDialog = new PrefixSettingsDialog(m_runConfig->prefix(), this);
        prefixSettingsDialog->exec(); });

    m_prefixToolsMenu = prefixMenu->addMenu(QIcon::fromTheme("tools"), tr("Tools"));

    m_prefixComponentsAction = m_prefixToolsMenu->addAction(QIcon::fromTheme("plugins"), tr("Install components"));
    connect(m_prefixComponentsAction, &QAction::triggered, this, [this]() {
        if (APP_SETTINGS->winetricksPath().isEmpty()) {
            QMessageBox::critical(this, tr("Opening error"), tr("\"winetricks\" not found! Please install this package to open this window"));
            return;
        }

        auto* prefixComponentsDialog = new PrefixComponentsDialog(m_runConfig->prefix(), this);
        prefixComponentsDialog->exec();
    });

    auto* winecfgAction = m_prefixToolsMenu->addAction(QIcon::fromTheme("wine-symbolic"), tr("Wine settings"));
    connect(winecfgAction, &QAction::triggered, this, [this]() { RUN_MANAGER->runWineCfg(m_runConfig->prefix()); });

    auto* explorerAction = m_prefixToolsMenu->addAction(QIcon::fromTheme("document-open-folder"), tr("Explorer"));
    connect(explorerAction, &QAction::triggered, this, [this]() { RUN_MANAGER->runExplorer(m_runConfig->prefix()); });

    auto* regeditAction = m_prefixToolsMenu->addAction(QIcon::fromTheme("view-list-text"), tr("Registry"));
    connect(regeditAction, &QAction::triggered, this, [this]() { RUN_MANAGER->runRegedit(m_runConfig->prefix()); });

    auto* uninstallerAction = m_prefixToolsMenu->addAction(QIcon::fromTheme("entry-delete"), tr("Remove programs"));
    connect(uninstallerAction, &QAction::triggered, this, [this]() { RUN_MANAGER->runUninstaller(m_runConfig->prefix()); });

    m_prefixOpenAction = prefixMenu->addAction(QIcon::fromTheme("document-open-folder"), tr("Open in files"));
    connect(m_prefixOpenAction, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(QUrl::fromLocalFile(m_runConfig->prefix()->path())); });

    prefixMenu->addSeparator();

    auto* prefixManageAction = prefixMenu->addAction(QIcon::fromTheme("view-list-text"), tr("Manage"));
    connect(prefixManageAction, &QAction::triggered, this, &MainWindow::onOpenPrefixListWidget);

    m_prefixMenuButton->setToolTip(tr("Open prefix menu"));
    m_prefixMenuButton->setIcon(QIcon::fromTheme("open-menu-symbolic"));
    m_prefixMenuButton->setMenu(prefixMenu);
    m_prefixMenuButton->setPopupMode(QToolButton::InstantPopup);
    environmentBoxLayout->addWidget(m_prefixMenuButton, 2, 1);

    auto* bottomPrefixLine = new QFrame(this);
    bottomPrefixLine->setFrameShape(QFrame::HLine);
    environmentBoxLayout->addWidget(bottomPrefixLine, 3, 0, 1, 2);

    auto* ctLabel = new QLabel(tr("Compatibility tool"), this);
    environmentBoxLayout->addWidget(ctLabel, 4, 0);

    auto* ctInstalledProxyModel = new CtInstalledProxyModel(this);
    m_ctComboBox->setPlaceholderText(tr("Install a new one →"));
    ctInstalledProxyModel->setSourceModel(CT_MODEL);
    m_ctComboBox->setModel(ctInstalledProxyModel);
    connect(ctInstalledProxyModel, &CtModel::rowsInserted, this, [this]() {
        if (m_ctComboBox->currentIndex() == -1 && CT_MODEL->rowCount() > 0) {
            m_ctComboBox->setCurrentIndex(0);
        }
    });
    environmentBoxLayout->addWidget(m_ctComboBox, 5, 0);

    m_ctWindowButton->setToolTip(tr("Open the Compatibility Tools window"));
    m_ctWindowButton->setIcon(QIcon::fromTheme("view-list-details"));
    connect(m_ctWindowButton, &QToolButton::clicked, this, &MainWindow::onOpenCtListWidget);
    environmentBoxLayout->addWidget(m_ctWindowButton, 5, 1);

    auto* bottomWidget = new QWidget(this);
    auto* bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(bottomWidget);

    auto* appSettingsWindowButton = new QToolButton(this);
    appSettingsWindowButton->setToolTip(tr("Application settings"));
    appSettingsWindowButton->setIcon(QIcon::fromTheme("configure"));
    connect(appSettingsWindowButton, &QToolButton::clicked, this, []() { openAppSettingsWindow(); });
    bottomLayout->addWidget(appSettingsWindowButton);

    auto* aboutAppButton = new QToolButton(this);
    aboutAppButton->setIcon(QIcon::fromTheme("help-about"));
    connect(aboutAppButton, &QToolButton::clicked, this, [this]() {
        auto* aboutDialog = new AboutAppDialog(this);
        aboutDialog->exec();
    });
    bottomLayout->addWidget(aboutAppButton);

    auto* versionLabel = new QLabel(tr("Version: %1").arg(APP_VERSION), this);
    versionLabel->setEnabled(false);
    bottomLayout->addWidget(versionLabel);

    connect(RUN_MANAGER, &RunManager::runningError, this, &MainWindow::onRunningError);
    connect(RUN_MANAGER, &RunManager::runningChanged, this, &MainWindow::onRunningChanged);

    setExecutablePath(exePath);

    connect(m_prefixTypeComboBox, &QComboBox::activated, this, &MainWindow::onPrefixTypeSelected);
    connect(m_prefixComboBox, &QComboBox::textActivated, this, &MainWindow::onPrefixTextSelected);
    connect(m_ctComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onCurrentCtIndexChanged);
}

void MainWindow::onPrefixTypeSelected(int index)
{
    switch (index) {
    case AppSettings::PrefixType::Individual:
        setIndividualPrefix();
        break;
    case AppSettings::PrefixType::Portable:
        setPortablePrefix();
        break;
    default:
        setSharedPrefix(PREFIX_MODEL->defaultPrefix());
        break;
    }
}

void MainWindow::setExecutablePath(const QString& exePath)
{
    m_runConfig->setExecutablePath(exePath);
    bool exeIsValid = m_runConfig->exeIsValid();

    m_exeNameLabel->setEnabled(exeIsValid);
    m_runStopAction->setEnabled(exeIsValid);
    m_exeNameLabel->setText(exeIsValid ? m_runConfig->exeName() : tr("The program is not selected"));

    // Set icon
    if (!exeIsValid || m_runConfig->exeIcon().isNull()) {
        m_exeIconLabel->setPixmap(m_unknownExePixmap);
    } else {
        m_exeIconLabel->setPixmap(m_runConfig->exeIcon().pixmap(m_exeIconSize));
    }

    newIndividualPrefixFromExe();
    newPortablePrefixFromExe();

    setPreferredPrefix();
}

void MainWindow::newIndividualPrefixFromExe()
{
    if (m_individualPrefix) {
        m_individualPrefixName.clear();
        m_individualPrefix->deleteLater();
        m_individualPrefix.clear(); // Clear pointer
    }

    if (m_runConfig->exeIsValid()) {
        m_individualPrefixName = m_runConfig->exeFile()->id();
        m_individualPrefix = new Prefix(PREFIXES_DIR.filePath(m_individualPrefixName), this);
    }
}

void MainWindow::newPortablePrefixFromExe()
{
    if (m_portablePrefix) {
        m_portablePrefix->deleteLater();
        m_portablePrefix.clear();
    }

    if (m_runConfig->exeIsValid()) {
        QString prefixPath = m_runConfig->exeFile()->dir().absoluteFilePath(APP_SETTINGS->portablePrefixName());
        m_portablePrefix = new Prefix(prefixPath, this);
    }
}

void MainWindow::setPreferredPrefix()
{
    if (m_runConfig->exeIsValid()) {
        // Prefer individual prefix if it exists
        if (PREFIX_MODEL->containsName(m_individualPrefixName)) {
            setIndividualPrefix();
            return;
        }

        if (m_portablePrefix && m_portablePrefix->exists()) {
            setPortablePrefix();
            return;
        }

        // If the executable file is inside the prefix, then prefer it
        const QString& exePath = m_runConfig->exePath();
        for (const auto& prefix : PREFIX_MODEL->list()) {
            if (exePath.startsWith(prefix->path())) {
                setSharedPrefix(prefix);
                return;
            }
        }
    }

    switch (APP_SETTINGS->prefixType()) {
    case AppSettings::PrefixType::Individual:
        setIndividualPrefix();
        break;
    case AppSettings::PrefixType::Portable:
        setPortablePrefix();
        break;
    default:
        setSharedPrefix(PREFIX_MODEL->defaultPrefix());
        break;
    }
}

void MainWindow::setSharedPrefix(Prefix* prefix)
{
    setPrefix(prefix);

    m_prefixComboBox->setCurrentText(prefix->name());
    m_prefixComboBox->setEnabled(true);
    m_prefixTypeComboBox->setCurrentIndex(AppSettings::PrefixType::Shared);
}

void MainWindow::setIndividualPrefix()
{
    setPrefix(m_individualPrefix);

    if (m_runConfig->exeIsValid()) {
        m_prefixComboBox->setPlaceholderText(m_individualPrefixName);
    } else {
        m_prefixComboBox->setPlaceholderText(tr("<Select a program>"));
    }

    m_prefixComboBox->setCurrentIndex(-1);
    m_prefixComboBox->setDisabled(true);
    m_prefixTypeComboBox->setCurrentIndex(AppSettings::PrefixType::Individual);
}

void MainWindow::setPortablePrefix()
{
    setPrefix(m_portablePrefix);

    if (m_runConfig->exeIsValid()) {
        m_prefixComboBox->setPlaceholderText(APP_SETTINGS->portablePrefixName());
    } else {
        m_prefixComboBox->setPlaceholderText(tr("<Select a program>"));
    }

    m_prefixComboBox->setCurrentIndex(-1);
    m_prefixComboBox->setDisabled(true);
    m_prefixTypeComboBox->setCurrentIndex(AppSettings::PrefixType::Portable);
}

void MainWindow::setPrefix(Prefix* prefix)
{
    if (prefix != m_runConfig->prefix()) {
        m_runConfig->setPrefix(prefix);
    }

    bool prefixNotNull = prefix != nullptr;

    m_prefixSettingsAction->setEnabled(prefixNotNull);
    m_prefixToolsMenu->setEnabled(prefixNotNull);
    m_prefixOpenAction->setEnabled(prefixNotNull);

    if (prefixNotNull) {
        m_prefixComboBox->setCurrentText(prefix->name());
    } else {
        m_prefixComboBox->setCurrentIndex(-1);
    }

    setPreferredCt();
}

void MainWindow::setPreferredCt()
{
    Ct* ct = nullptr;
    if (m_runConfig->prefix() != nullptr) {
        ct = CT_MODEL->getByPath(m_runConfig->prefix()->settings()->ctPath());
    }

    if (ct == nullptr) {
        Ct* defaultCt = CT_MODEL->defaultCt();
        m_runConfig->setCt(defaultCt);
        if (defaultCt != nullptr) {
            m_ctComboBox->setCurrentIndex(CT_MODEL->indexOf(defaultCt));
        }
    } else {
        m_runConfig->setCt(ct);
        m_ctComboBox->setCurrentIndex(CT_MODEL->indexOf(ct));
    }
}

void MainWindow::onPrefixTextSelected(const QString& prefixName)
{
    if (!prefixName.isEmpty()) {
        setPrefix(PREFIX_MODEL->getByName(prefixName));
    }
}

void MainWindow::onCurrentCtIndexChanged(int index)
{
    m_runConfig->setCt(CT_MODEL->getByIndex(index));
}

void MainWindow::onOpenPrefixListWidget()
{
    auto* prefixListWidget = new PrefixListWidget();
    prefixListWidget->show();
}

void MainWindow::onOpenCtListWidget()
{
    auto* ctListWidget = new CtListWidget();
    ctListWidget->resize(400, ctListWidget->height());
    ctListWidget->show();
}

void MainWindow::openAppSettingsWindow()
{
    auto* appSettingsWindow = new AppSettingsWindow();
    appSettingsWindow->show();
}

void MainWindow::onExeSelectionClicked()
{
    QFileInfo exeFileInfo(
        QFileDialog::getOpenFileName(
            this,
            tr("Select the executable file"),
            m_lastSearchPath,
            tr("Executable files (*.exe *.msi *.bat);;All files (*.*)")));

    if (exeFileInfo.exists()) {
        m_lastSearchPath = exeFileInfo.dir().path();
        setExecutablePath(exeFileInfo.absoluteFilePath());
    }
}

void MainWindow::onRunStopTriggered()
{
    if (RUN_MANAGER->isRunning()) {
        RUN_MANAGER->stop();
    } else {
        RUN_MANAGER->run();
    }
}

void MainWindow::onEditShortcutsTriggered()
{
    auto* shortcutDialog = new EditShortcutsDialog(m_runConfig->exePath(), m_runConfig->prefix(), this);
    shortcutDialog->show();
}

void MainWindow::onRunningError(RunManager::RunningError error, const QString& errorText)
{
    static QString errorTitle = tr("Running error");
    switch (error) {
    case RunManager::RunningError::AlreadyRunning:
        QMessageBox::critical(this, errorTitle, tr("The executable file is currently running"));
        break;
    case RunManager::RunningError::InvalidExecutable:
        QMessageBox::critical(this, errorTitle, tr("The executable file is not valid"));
        break;
    case RunManager::RunningError::PrefixWriteError:
        QMessageBox::critical(this, errorTitle, tr("Failed to write prefix"));
        break;
    case RunManager::RunningError::InvalidCt: {
        auto answer = QMessageBox::question(
            this,
            errorTitle,
            tr("The required compatibility tool is missing, open window to manage?"));
        if (answer == QMessageBox::Yes) {
            onOpenCtListWidget();
        }
    } break;
    case RunManager::RunningError::NoUmu:
        QMessageBox::critical(this, errorTitle, tr("\"umu-run\" not found"));
        break;
    case RunManager::RunningError::NoWinetricks:
        QMessageBox::critical(this, errorTitle, tr("\"winetricks\" not found"));
        break;
    case RunManager::RunningError::FailedToStart:
        QMessageBox::critical(this, errorTitle, tr("Failed to start process: %1").arg(errorText));
        break;
    case RunManager::RunningError::Crashed:
        QMessageBox::critical(this, errorTitle, tr("Process error: %1").arg(errorText));
        break;
    case RunManager::RunningError::Timedout:
        QMessageBox::critical(this, errorTitle, tr("Process timeout: %1").arg(errorText));
        break;
    case RunManager::RunningError::ReadError:
        QMessageBox::critical(this, errorTitle, tr("Process read error: %1").arg(errorText));
        break;
    case RunManager::RunningError::WriteError:
        QMessageBox::critical(this, errorTitle, tr("Process write error: %1").arg(errorText));
        break;
    default:
        QMessageBox::critical(this, errorTitle, tr("Unknown error: %1").arg(errorText));
        break;
    }
}

void MainWindow::onRunningChanged(bool isRunning)
{
    m_exeSelectionButton->setDisabled(isRunning);
    m_runStopButton->setIcon(isRunning ? QIcon::fromTheme("media-playback-stop") : QIcon::fromTheme("media-playback-start"));
    m_runStopAction->setText(isRunning ? tr("Stop") : tr("Run"));
    setHidden(isRunning);
}
