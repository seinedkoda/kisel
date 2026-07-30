#include "main_window.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>

#include "app_settings.hpp"
#include "app_settings_window.hpp"
#include "ct_model.hpp"
#include "ct_window.hpp"
#include "executable_file.hpp"
#include "prefix_components_dialog.hpp"
#include "prefix_model.hpp"
#include "prefix_settings_dialog.hpp"
#include "prefix_window.hpp"
#include "shortcut_dialog.hpp"

using namespace kisel;

MainWindow::MainWindow(ProcessManager& processManager, const QString& exePath)
    : QMainWindow(nullptr)
    , m_processManager(processManager)
    , m_exeIconLabel(new QLabel(this))
    , m_exeNameLabel(new QLabel(tr("The program is not selected"), this))
    , m_playStopButton(new QPushButton(tr("Start"), this))
    , m_createExeShortcutButton(new QToolButton(this))
    , m_exeSelectionButton(new QToolButton(this))
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
    auto* layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    auto* exeWidget = new QWidget(this);
    auto* exeLayout = new QGridLayout(exeWidget);
    exeLayout->setAlignment(Qt::AlignLeft);
    layout->addWidget(exeWidget);

    m_exeIconLabel->setFixedSize(64, 64);
    m_exeIconLabel->setScaledContents(true);
    m_exeIconLabel->setPixmap(m_unknownExePixmap);
    exeLayout->addWidget(m_exeIconLabel, 0, 0, 2, 1, Qt::AlignLeft);

    exeLayout->addWidget(m_exeNameLabel, 0, 1, Qt::AlignLeft);

    auto* exeActionsWidget = new QWidget(this);
    exeLayout->addWidget(exeActionsWidget, 1, 1, Qt::AlignLeft);
    auto* exeActionsLayout = new QHBoxLayout(exeActionsWidget);
    exeActionsLayout->setContentsMargins(0, 0, 0, 0);

    m_playStopButton->setIcon(QIcon::fromTheme("media-playback-start"));
    connect(m_playStopButton, &QPushButton::clicked, this, &MainWindow::onPlayStopButtonClicked);
    exeActionsLayout->addWidget(m_playStopButton, Qt::AlignLeft);

    m_createExeShortcutButton->setToolTip(tr("Create shortcut"));
    m_createExeShortcutButton->setIcon(QIcon::fromTheme("link"));
    m_createExeShortcutButton->setEnabled(false);
    connect(m_createExeShortcutButton, &QToolButton::clicked, this, &MainWindow::onCreateExeShortcutButtonClicked);
    exeActionsLayout->addWidget(m_createExeShortcutButton, Qt::AlignLeft);

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

    m_prefixComboBox->setModel(PREFIX_MODEL);
    environmentBoxLayout->addWidget(m_prefixComboBox, 1, 0);

    auto* prefixMenu = new QMenu(this);

    m_prefixSettingsAction = new QAction(QIcon::fromTheme("view-process-system"), tr("Configure"), prefixMenu);
    connect(m_prefixSettingsAction, &QAction::triggered, this, [this]() {
        auto* prefixSettingsDialog = new PrefixSettingsDialog(*m_exeFile->prefix(), this);
        prefixSettingsDialog->exec(); });
    prefixMenu->addAction(m_prefixSettingsAction);

    auto* toolsMenu = prefixMenu->addMenu(QIcon::fromTheme("tools"), tr("Tools"));

    m_prefixComponentsAction = new QAction(QIcon::fromTheme("plugins"), tr("Install components"), toolsMenu);
    connect(m_prefixComponentsAction, &QAction::triggered, this, [this]() {
        if (APP_SETTINGS->winetricksPath().isEmpty()) {
            QMessageBox::critical(this, tr("Opening error"), tr("\"winetricks\" not found! Please install this package to open this window"));
            return;
        }

        auto* prefixComponentsDialog = new PrefixComponentsDialog(*m_exeFile->prefix(), this);
        prefixComponentsDialog->exec();
    });
    toolsMenu->addAction(m_prefixComponentsAction);

    auto* winecfgAction = new QAction(QIcon::fromTheme("wine"), tr("Wine settings"), toolsMenu);
    connect(winecfgAction, &QAction::triggered, this, [this]() { m_processManager.runWineCfg(m_exeFile->prefix()); });
    toolsMenu->addAction(winecfgAction);

    auto* explorerAction = new QAction(QIcon::fromTheme("document-open-folder"), tr("Explorer"), toolsMenu);
    connect(explorerAction, &QAction::triggered, this, [this]() { m_processManager.runExplorer(m_exeFile->prefix()); });
    toolsMenu->addAction(explorerAction);

    auto* regeditAction = new QAction(QIcon::fromTheme("view-list-text"), tr("Registry"), toolsMenu);
    connect(regeditAction, &QAction::triggered, this, [this]() { m_processManager.runRegedit(m_exeFile->prefix()); });
    toolsMenu->addAction(regeditAction);

    auto* uninstallerAction = new QAction(QIcon::fromTheme("trash-empty"), tr("Remove programs"), toolsMenu);
    connect(uninstallerAction, &QAction::triggered, this, [this]() { m_processManager.runUninstaller(m_exeFile->prefix()); });
    toolsMenu->addAction(uninstallerAction);

    auto* prefixManageAction = new QAction(QIcon::fromTheme("view-list-text"), tr("Manage"), prefixMenu);
    connect(prefixManageAction, &QAction::triggered, this, []() { openPrefixWindow(); });
    prefixMenu->addAction(prefixManageAction);

    m_prefixOpenAction = new QAction(QIcon::fromTheme("document-open-folder"), tr("Open"), prefixMenu);
    connect(m_prefixOpenAction, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(QUrl::fromLocalFile(m_exeFile->prefix()->path())); });
    prefixMenu->addAction(m_prefixOpenAction);

    m_prefixMenuButton->setToolTip(tr("Open prefix menu"));
    m_prefixMenuButton->setIcon(QIcon::fromTheme("application-menu"));
    connect(m_prefixMenuButton, &QToolButton::clicked, this, [this, prefixMenu]() {
        prefixMenu->exec(QCursor::pos());
    });
    environmentBoxLayout->addWidget(m_prefixMenuButton, 1, 1);

    auto* ctLabel = new QLabel(tr("Compatibility tool"), this);
    environmentBoxLayout->addWidget(ctLabel, 2, 0);

    m_ctComboBox->setModel(CT_MODEL);
    environmentBoxLayout->addWidget(m_ctComboBox, 3, 0);

    m_ctWindowButton->setToolTip(tr("Open the Compatibility Tools window"));
    m_ctWindowButton->setIcon(QIcon::fromTheme("window"));
    connect(m_ctWindowButton, &QToolButton::clicked, this, [this]() { openCtWindow(); });
    environmentBoxLayout->addWidget(m_ctWindowButton, 3, 1);

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

    connect(&m_processManager, &ProcessManager::runningError, this, &MainWindow::onRunningError);
    connect(&m_processManager, &ProcessManager::runningChanged, this, &MainWindow::onRunningChanged);

    setExecutablePath(exePath);
    setPreferredPrefix();

    connect(m_prefixComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentPrefixTextChanged);
    connect(m_ctComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onCurrentCtIndexChanged);
}

void MainWindow::setExecutablePath(const QString& exePath)
{
    if (m_exeFile == nullptr) {
        m_exeFile = new ExecutableFile(exePath, this);
    } else {
        m_exeFile->setPath(exePath);
    }

    bool exeIsValid = m_exeFile->isValid();
    m_exeNameLabel->setEnabled(exeIsValid);
    m_playStopButton->setEnabled(exeIsValid);
    m_createExeShortcutButton->setEnabled(exeIsValid);
    m_exeNameLabel->setText(exeIsValid ? m_exeFile->name() : tr("The program is not selected"));
    if (m_exeFile->icon().isNull()) {
        m_exeIconLabel->setPixmap(m_unknownExePixmap);
    } else {
        m_exeIconLabel->setPixmap(m_exeFile->icon().pixmap(64, 64));
    }
}

void MainWindow::setPreferredPrefix()
{
    if (m_exeFile->isValid()) {
        const QString& exePath = m_exeFile->path();

        // If the executable file is inside the prefix, then prefer it
        for (const auto& prefix : PREFIX_MODEL->list()) {
            if (exePath.startsWith(prefix->path())) {
                setPrefix(prefix);
                return;
            }
        }
    }

    setPrefix(PREFIX_MODEL->defaultPrefix());
}

void MainWindow::onCurrentPrefixTextChanged(const QString& prefixName)
{
    setPrefix(PREFIX_MODEL->forName(prefixName));
}

void MainWindow::setPrefix(Prefix* prefix)
{
    m_exeFile->setPrefix(prefix);
    bool prefixExists = prefix != nullptr;

    if (prefixExists) {
        m_prefixComboBox->setCurrentText(prefix->name());

        Ct* ct = prefix->ct();
        if (ct != nullptr) {
            m_ctComboBox->setCurrentText(ct->name());
        }

        PrefixSettings* prefixSettings = prefix->settings();
    }

    m_prefixSettingsAction->setEnabled(prefixExists);
    m_prefixComponentsAction->setEnabled(prefixExists);
    m_prefixOpenAction->setEnabled(prefixExists);
}

void MainWindow::onCurrentCtIndexChanged(int index)
{
    Ct* ct = CT_MODEL->forIndex(index);
    if (m_exeFile->prefix() != nullptr) {
        m_exeFile->prefix()->setCt(ct);
    }
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

void MainWindow::onPlayStopButtonClicked()
{
    if (m_processManager.isRunning()) {
        m_processManager.stop();
    } else if (m_exeFile->prefix() == nullptr) {
        auto answer = QMessageBox::question(
            this,
            tr("Launch error"),
            tr("The required prefix is ​​missing, open window to manage?"));
        if (answer == QMessageBox::Yes) {
            openPrefixWindow();
        }
        return;
    } else if (m_exeFile->prefix()->ct() == nullptr) {
        auto answer = QMessageBox::question(
            this,
            tr("Launch error"),
            tr("The required compatibility tool is missing, open window to manage?"));
        if (answer == QMessageBox::Yes) {
            openCtWindow();
        }
        return;
    } else {
        m_processManager.run(*m_exeFile);
    }
}

void MainWindow::onCreateExeShortcutButtonClicked()
{
    auto* shortcutDialog = new ShortcutDialog(m_exeFile, this);
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
    case ProcessManager::RunningError::InvalidPrefix: {
        auto answer = QMessageBox::question(
            this,
            errorTitle,
            tr("The required prefix is ​​missing, open window to manage?"));
        if (answer == QMessageBox::Yes) {
            openPrefixWindow();
        }
    } break;
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
    m_playStopButton->setIcon(isRunning ? QIcon::fromTheme("media-playback-stop") : QIcon::fromTheme("media-playback-start"));
    m_playStopButton->setText(isRunning ? tr("Stop") : tr("Start"));
    m_createExeShortcutButton->setDisabled(isRunning);
    setHidden(isRunning);
}
