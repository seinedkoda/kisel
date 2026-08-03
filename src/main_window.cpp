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

#include "app_settings.hpp"
#include "app_settings_window.hpp"
#include "ct_model.hpp"
#include "ct_window.hpp"
#include "executable_file.hpp"
#include "prefix.hpp"
#include "prefix_components_dialog.hpp"
#include "prefix_model.hpp"
#include "prefix_settings.hpp"
#include "prefix_settings_dialog.hpp"
#include "prefix_window.hpp"
#include "run_config.hpp"
#include "shortcut_dialog.hpp"

using namespace kisel;

MainWindow::MainWindow(ProcessManager* processManager, const QString& exePath)
    : QMainWindow(nullptr)
    , m_processManager(processManager)
    , m_exeFile(new ExecutableFile(exePath, this))
    , m_runConfig(new RunConfig(this))
    , m_exeIconLabel(new QLabel(this))
    , m_exeNameLabel(new QLabel(tr("The program is not selected"), this))
    , m_runStopAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("Run"), this))
    , m_runStopButton(new QToolButton(this))
    , m_exeSelectionButton(new QToolButton(this))
    , m_individualPrefixCheckBox(new QCheckBox(tr("Create from program file"), this))
    , m_prefixComboBox(new QComboBox(this))
    , m_prefixMenuButton(new QToolButton(this))
    , m_ctComboBox(new QComboBox(this))
    , m_ctWindowButton(new QToolButton(this))
    , m_appSettingsWindowButton(new QToolButton(this))
{
    setWindowTitle(tr("Kisel"));
    setWindowIcon(QIcon(":/icons/kisel.svg"));
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

    auto* createShortcutAction = new QAction(QIcon::fromTheme("link"), tr("Create shortcut"), this);
    connect(createShortcutAction, &QAction::triggered, this, &MainWindow::onCreateShortcutTriggered);
    exeMenu->addAction(createShortcutAction);

    auto* clearExeAction = new QAction(QIcon::fromTheme("edit-clear"), tr("Clear"), this);
    connect(clearExeAction, &QAction::triggered, this, [this]() { setExecutablePath(""); });
    exeMenu->addAction(clearExeAction);

    connect(m_runStopAction, &QAction::triggered, this, &MainWindow::onRunStopTriggered);

    m_runStopButton->setDefaultAction(m_runStopAction);
    m_runStopButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_runStopButton->setMenu(exeMenu);
    m_runStopButton->setPopupMode(QToolButton::MenuButtonPopup);
    exeActionsLayout->addWidget(m_runStopButton, Qt::AlignLeft);

    m_exeSelectionButton->setToolTip(tr("Select executable file"));
    m_exeSelectionButton->setIcon(QIcon::fromTheme("search"));
    connect(m_exeSelectionButton, &QToolButton::clicked, this, &MainWindow::onExeSelectionClicked);
    exeActionsLayout->addWidget(m_exeSelectionButton, Qt::AlignLeft);

    auto* environmentBox = new QGroupBox(tr("Environment"), this);
    environmentBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(environmentBox);
    auto* environmentBoxLayout = new QGridLayout(environmentBox);
    environmentBoxLayout->setAlignment(Qt::AlignTop);

    auto* prefixLabel = new QLabel(tr("Prefix"), this);
    environmentBoxLayout->addWidget(prefixLabel, 0, 0);

    m_individualPrefixCheckBox->setChecked(APP_SETTINGS->useIndividualPrefix());
    environmentBoxLayout->addWidget(m_individualPrefixCheckBox, 1, 0);

    m_prefixComboBox->setModel(PREFIX_MODEL);
    environmentBoxLayout->addWidget(m_prefixComboBox, 2, 0);

    auto* prefixMenu = new QMenu(m_prefixMenuButton);

    m_prefixSettingsAction = new QAction(QIcon::fromTheme("view-process-system"), tr("Configure"), prefixMenu);
    connect(m_prefixSettingsAction, &QAction::triggered, this, [this]() {
        auto* prefixSettingsDialog = new PrefixSettingsDialog(m_runConfig->prefix(), this);
        prefixSettingsDialog->exec(); });
    prefixMenu->addAction(m_prefixSettingsAction);

    m_prefixToolsMenu = prefixMenu->addMenu(QIcon::fromTheme("tools"), tr("Tools"));

    m_prefixComponentsAction = new QAction(QIcon::fromTheme("plugins"), tr("Install components"), m_prefixToolsMenu);
    connect(m_prefixComponentsAction, &QAction::triggered, this, [this]() {
        if (APP_SETTINGS->winetricksPath().isEmpty()) {
            QMessageBox::critical(this, tr("Opening error"), tr("\"winetricks\" not found! Please install this package to open this window"));
            return;
        }

        auto* prefixComponentsDialog = new PrefixComponentsDialog(*m_runConfig->prefix(), this);
        prefixComponentsDialog->exec();
    });
    m_prefixToolsMenu->addAction(m_prefixComponentsAction);

    auto* winecfgAction = new QAction(QIcon::fromTheme("wine"), tr("Wine settings"), m_prefixToolsMenu);
    connect(winecfgAction, &QAction::triggered, this, [this]() { m_processManager->runWineCfg(m_runConfig->prefix()); });
    m_prefixToolsMenu->addAction(winecfgAction);

    auto* explorerAction = new QAction(QIcon::fromTheme("document-open-folder"), tr("Explorer"), m_prefixToolsMenu);
    connect(explorerAction, &QAction::triggered, this, [this]() { m_processManager->runExplorer(m_runConfig->prefix()); });
    m_prefixToolsMenu->addAction(explorerAction);

    auto* regeditAction = new QAction(QIcon::fromTheme("view-list-text"), tr("Registry"), m_prefixToolsMenu);
    connect(regeditAction, &QAction::triggered, this, [this]() { m_processManager->runRegedit(m_runConfig->prefix()); });
    m_prefixToolsMenu->addAction(regeditAction);

    auto* uninstallerAction = new QAction(QIcon::fromTheme("trash-empty"), tr("Remove programs"), m_prefixToolsMenu);
    connect(uninstallerAction, &QAction::triggered, this, [this]() { m_processManager->runUninstaller(m_runConfig->prefix()); });
    m_prefixToolsMenu->addAction(uninstallerAction);

    m_prefixOpenAction = new QAction(QIcon::fromTheme("document-open-folder"), tr("Open"), prefixMenu);
    connect(m_prefixOpenAction, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(QUrl::fromLocalFile(m_runConfig->prefix()->path())); });
    prefixMenu->addAction(m_prefixOpenAction);

    prefixMenu->addSeparator();

    auto* prefixManageAction = new QAction(QIcon::fromTheme("view-list-text"), tr("Manage"), prefixMenu);
    connect(prefixManageAction, &QAction::triggered, this, []() { openPrefixWindow(); });
    prefixMenu->addAction(prefixManageAction);

    m_prefixMenuButton->setToolTip(tr("Open prefix menu"));
    m_prefixMenuButton->setIcon(QIcon::fromTheme("application-menu"));
    m_prefixMenuButton->setMenu(prefixMenu);
    m_prefixMenuButton->setPopupMode(QToolButton::InstantPopup);
    environmentBoxLayout->addWidget(m_prefixMenuButton, 2, 1);

    auto* ctLabel = new QLabel(tr("Compatibility tool"), this);
    environmentBoxLayout->addWidget(ctLabel, 3, 0);

    m_ctComboBox->setModel(CT_MODEL);
    environmentBoxLayout->addWidget(m_ctComboBox, 4, 0);

    m_ctWindowButton->setToolTip(tr("Open the Compatibility Tools window"));
    m_ctWindowButton->setIcon(QIcon::fromTheme("window"));
    connect(m_ctWindowButton, &QToolButton::clicked, this, [this]() { openCtWindow(); });
    environmentBoxLayout->addWidget(m_ctWindowButton, 4, 1);

    auto* bottomWidget = new QWidget(this);
    auto* bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(bottomWidget);

    m_appSettingsWindowButton->setToolTip(tr("Application settings"));
    m_appSettingsWindowButton->setIcon(QIcon::fromTheme("configure"));
    connect(m_appSettingsWindowButton, &QToolButton::clicked, this, []() { openAppSettingsWindow(); });
    bottomLayout->addWidget(m_appSettingsWindowButton);

    auto* versionLabel = new QLabel(tr("Version: %1").arg(APP_VERSION), this);
    versionLabel->setEnabled(false);
    bottomLayout->addWidget(versionLabel);

    connect(m_processManager, &ProcessManager::runningError, this, &MainWindow::onRunningError);
    connect(m_processManager, &ProcessManager::runningChanged, this, &MainWindow::onRunningChanged);

    setExecutablePath(exePath);

    connect(m_individualPrefixCheckBox, &QCheckBox::clicked, this, &MainWindow::individualPrefixStateChanged);
    connect(m_prefixComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentPrefixTextChanged);
    connect(m_ctComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onCurrentCtIndexChanged);
}

void MainWindow::individualPrefixStateChanged(bool checked)
{
    if (checked) {
        setPrefix(m_individualPrefix);
    } else {
        setPrefix(PREFIX_MODEL->defaultPrefix());
    }
}

void MainWindow::setExecutablePath(const QString& exePath)
{
    m_exeFile->setPath(exePath);

    bool exeIsValid = m_exeFile->isValid();
    m_exeNameLabel->setEnabled(exeIsValid);
    m_runStopAction->setEnabled(exeIsValid);
    m_exeNameLabel->setText(exeIsValid ? m_exeFile->name() : tr("The program is not selected"));
    if (!exeIsValid || m_exeFile->icon().isNull()) {
        m_exeIconLabel->setPixmap(m_unknownExePixmap);
    } else {
        m_exeIconLabel->setPixmap(m_exeFile->icon().pixmap(m_exeIconSize));
    }

    if (m_individualPrefix) {
        m_individualPrefixName.clear();
        m_individualPrefix->deleteLater();
    }

    if (exeIsValid) {
        m_individualPrefixName = Prefix::generatePrefixNameFromFile(m_exeFile->path());
        m_individualPrefix = new Prefix(m_individualPrefixName, this);
        m_prefixComboBox->setPlaceholderText(m_individualPrefixName);
    } else {
        m_prefixComboBox->setPlaceholderText(tr("<Select a program>"));
    }

    setPreferredPrefix();
}

void MainWindow::setPreferredPrefix()
{
    if (m_exeFile->isValid()) {
        // Prefer individual prefix if it exists
        if (PREFIX_MODEL->containsName(m_individualPrefixName)) {
            setPrefix(m_individualPrefix);
            return;
        }

        // If the executable file is inside the prefix, then prefer it
        const QString& exePath = m_exeFile->path();
        for (const auto& prefix : PREFIX_MODEL->list()) {
            if (exePath.startsWith(prefix->path())) {
                setPrefix(prefix);
                return;
            }
        }
    }

    if (m_individualPrefixCheckBox->isChecked()) {
        setPrefix(m_individualPrefix);
    } else {
        setPrefix(PREFIX_MODEL->defaultPrefix());
    }
}

void MainWindow::setPrefix(Prefix* prefix)
{
    if (prefix == m_runConfig->prefix()) {
        return;
    }

    m_runConfig->setPrefix(prefix);

    bool prefixExists = prefix != nullptr;
    bool isIndividualPrefix = prefix == m_individualPrefix;

    if (isIndividualPrefix) {
        m_prefixComboBox->setCurrentIndex(-1);
    } else if (prefixExists) {
        m_prefixComboBox->setCurrentText(prefix->name());
    }

    m_prefixSettingsAction->setEnabled(prefixExists);
    m_prefixToolsMenu->setEnabled(prefixExists);
    m_prefixOpenAction->setEnabled(prefixExists);

    m_individualPrefixCheckBox->setChecked(isIndividualPrefix);
    m_prefixComboBox->setDisabled(isIndividualPrefix);

    setPreferredCt();
}

void MainWindow::setPreferredCt()
{
    Ct* ct = nullptr;
    if (m_runConfig->prefix() != nullptr) {
        ct = CT_MODEL->forPath(m_runConfig->prefix()->settings()->ctPath());
    }

    if (ct == nullptr) {
        Ct* defaultCt = CT_MODEL->defaultCt();
        m_runConfig->setCt(defaultCt);
        if (defaultCt != nullptr) {
            m_ctComboBox->setCurrentIndex(CT_MODEL->ctIndex(defaultCt));
        }
    } else {
        m_runConfig->setCt(ct);
        m_ctComboBox->setCurrentIndex(CT_MODEL->ctIndex(ct));
    }
}

void MainWindow::onCurrentPrefixTextChanged(const QString& prefixName)
{
    if (!prefixName.isEmpty()) {
        setPrefix(PREFIX_MODEL->forName(prefixName));
    }
}

void MainWindow::onCurrentCtIndexChanged(int index)
{
    m_runConfig->setCt(CT_MODEL->forIndex(index));
}

void MainWindow::openPrefixWindow()
{
    auto* prefixWindow = new PrefixWindow();
    prefixWindow->show();
}

void MainWindow::openCtWindow()
{
    auto* ctWindow = new CtWindow();
    ctWindow->show();
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
            QDir::homePath(),
            tr("Executable files (*.exe);;All files (*.*)")));

    if (exeFileInfo.exists()) {
        setExecutablePath(exeFileInfo.absoluteFilePath());
    }
}

void MainWindow::onRunStopTriggered()
{
    if (m_processManager->isRunning()) {
        m_processManager->stop();
    } else {
        m_processManager->run(*m_exeFile, m_runConfig);
    }
}

void MainWindow::onCreateShortcutTriggered()
{
    auto* shortcutDialog = new ShortcutDialog(m_exeFile, *m_runConfig, this);
    shortcutDialog->show();
}

void MainWindow::onRunningError(ProcessManager::RunningError error, const QString& errorText)
{
    static QString errorTitle = tr("Running error");
    switch (error) {
    case ProcessManager::RunningError::AlreadyRunning:
        QMessageBox::critical(this, errorTitle, tr("The executable file is currently running"));
        break;
    case ProcessManager::RunningError::InvalidExecutable:
        QMessageBox::critical(this, errorTitle, tr("The executable file is not valid"));
        break;
    case ProcessManager::RunningError::PrefixWriteError:
        QMessageBox::critical(this, errorTitle, tr("Failed to write prefix"));
        break;
    case ProcessManager::RunningError::InvalidCt: {
        auto answer = QMessageBox::question(
            this,
            errorTitle,
            tr("The required compatibility tool is missing, open window to manage?"));
        if (answer == QMessageBox::Yes) {
            openCtWindow();
        }
    } break;
    case ProcessManager::RunningError::NoUmu:
        QMessageBox::critical(this, errorTitle, tr("\"umu-run\" not found"));
        break;
    case ProcessManager::RunningError::NoWinetricks:
        QMessageBox::critical(this, errorTitle, tr("\"winetricks\" not found"));
        break;
    case ProcessManager::RunningError::FailedToStart:
        QMessageBox::critical(this, errorTitle, tr("Failed to start process: %1").arg(errorText));
        break;
    case ProcessManager::RunningError::Crashed:
        QMessageBox::critical(this, errorTitle, tr("Process error: %1").arg(errorText));
        break;
    case ProcessManager::RunningError::Timedout:
        QMessageBox::critical(this, errorTitle, tr("Process timeout: %1").arg(errorText));
        break;
    case ProcessManager::RunningError::ReadError:
        QMessageBox::critical(this, errorTitle, tr("Process read error: %1").arg(errorText));
        break;
    case ProcessManager::RunningError::WriteError:
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
