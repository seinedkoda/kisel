#include "main_window.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QToolBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <qnamespace.h>

#include "app_settings_window.hpp"
#include "ct_model.hpp"
#include "ct_window.hpp"
#include "executable_file.hpp"
#include "prefix_model.hpp"
#include "prefix_window.hpp"
#include "shortcut_dialog.hpp"

using namespace kisel;

MainWindow::MainWindow(ProcessManager& processManager, const QString& exePath)
    : QMainWindow(nullptr)
    , m_processManager(processManager)
    , m_playStopButton(new QPushButton(tr("Start"), this))
    , m_exeNameLabel(new QLabel(tr("The program is not selected"), this))
    , m_prefixWindowButton(new QToolButton(this))
    , m_exeSelectionButton(new QToolButton(this))
    , m_prefixComboBox(new QComboBox(this))
    , m_ctComboBox(new QComboBox(this))
    , m_ctWindowButton(new QToolButton(this))
    , m_settingsWindowButton(new QToolButton(this))
    , m_exeIconLabel(new QLabel(this))
    , m_createExeShortcutButton(new QToolButton(this))
    , m_mangohudCheckBox(new QCheckBox("MangoHud", this))
    , m_obsVkCaptureCheckBox(new QCheckBox("OBS Vulkan Capture", this))
    , m_xaliaCheckBox(new QCheckBox("Xalia", this))
    , m_waylandCheckBox(new QCheckBox(tr("Enable Wayland driver"), this))
    , m_useSteamCheckBox(new QCheckBox(tr("Steam Simulation"), this))
    , m_wow64CheckBox(new QCheckBox(tr("Enable WOW64")))
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

    m_createExeShortcutButton->setIcon(QIcon::fromTheme("link"));
    m_createExeShortcutButton->setToolTip(tr("Create shortcut"));
    m_createExeShortcutButton->setEnabled(false);
    connect(m_createExeShortcutButton, &QToolButton::clicked, this, &MainWindow::onCreateExeShortcutButtonClicked);
    exeActionsLayout->addWidget(m_createExeShortcutButton, Qt::AlignLeft);

    m_exeSelectionButton->setIcon(QIcon::fromTheme("search"));
    connect(m_exeSelectionButton, &QToolButton::clicked, this, &MainWindow::onExeSelectionClicked);
    exeActionsLayout->addWidget(m_exeSelectionButton, Qt::AlignLeft);

    auto* generalTab = new QWidget(this);
    auto* generalTabLayout = new QGridLayout(generalTab);
    generalTabLayout->setAlignment(Qt::AlignTop);

    auto* prefixLabel = new QLabel(tr("Prefix"), this);
    generalTabLayout->addWidget(prefixLabel, 0, 0);

    m_prefixComboBox->setModel(PREFIX_MODEL);
    generalTabLayout->addWidget(m_prefixComboBox, 1, 0);

    m_prefixWindowButton->setIcon(QIcon::fromTheme("window"));
    connect(m_prefixWindowButton, &QToolButton::clicked, this, []() { openPrefixWindow(); });
    generalTabLayout->addWidget(m_prefixWindowButton, 1, 1);

    auto* ctLabel = new QLabel(tr("Compatibility tool"), this);
    generalTabLayout->addWidget(ctLabel, 2, 0);
    m_ctComboBox->setModel(CT_MODEL);

    generalTabLayout->addWidget(m_ctComboBox, 3, 0);

    m_ctWindowButton->setIcon(QIcon::fromTheme("window"));
    connect(m_ctWindowButton, &QToolButton::clicked, this, [this]() { openCtWindow(); });
    generalTabLayout->addWidget(m_ctWindowButton, 3, 1);

    auto* advancedTab = new QWidget(this);
    auto* advancedTabLayout = new QGridLayout(advancedTab);
    advancedTabLayout->setAlignment(Qt::AlignTop);

    connect(m_mangohudCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_exeFile->prefix()->settings()->setMangoHudEnabled(checked);
    });
    advancedTabLayout->addWidget(m_mangohudCheckBox);

    connect(m_obsVkCaptureCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_exeFile->prefix()->settings()->setObsVkCaptureEnabled(checked);
    });
    advancedTabLayout->addWidget(m_obsVkCaptureCheckBox);

    connect(m_xaliaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_exeFile->prefix()->settings()->setXaliaEnabled(checked);
    });
    advancedTabLayout->addWidget(m_xaliaCheckBox);

    connect(m_waylandCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_exeFile->prefix()->settings()->setWaylandEnabled(checked);
    });
    advancedTabLayout->addWidget(m_waylandCheckBox);

    connect(m_useSteamCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_exeFile->prefix()->settings()->setSteamEnabled(checked);
    });
    advancedTabLayout->addWidget(m_useSteamCheckBox);

    connect(m_wow64CheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_exeFile->prefix()->settings()->setWow64Enabled(checked);
    });
    advancedTabLayout->addWidget(m_wow64CheckBox);

    auto* toolBox = new QToolBox(this);
    toolBox->addItem(generalTab, QIcon::fromTheme("user-home"), tr("General"));
    toolBox->addItem(advancedTab, QIcon::fromTheme("view-process-system"), tr("Advanced"));
    layout->addWidget(toolBox);

    auto* bottomWidget = new QWidget(this);
    auto* bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(bottomWidget);

    m_settingsWindowButton->setIcon(QIcon::fromTheme("configure"));
    connect(m_settingsWindowButton, &QToolButton::clicked, this, []() { openAppSettingsWindow(); });
    bottomLayout->addWidget(m_settingsWindowButton);

    auto* versionLabel = new QLabel(tr("Version: %1").arg(APP_VERSION), this);
    versionLabel->setEnabled(false);
    bottomLayout->addWidget(versionLabel);

    connect(&m_processManager, &ProcessManager::processError, this, &MainWindow::onProcessError);
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

        m_mangohudCheckBox->setChecked(prefixSettings->mangoHudEnabled());
        m_obsVkCaptureCheckBox->setChecked(prefixSettings->obsVkCaptureEnabled());
        m_xaliaCheckBox->setChecked(prefixSettings->xaliaEnabled());
        m_waylandCheckBox->setChecked(prefixSettings->waylandEnabled());
        m_useSteamCheckBox->setChecked(prefixSettings->steamEnabled());
        m_wow64CheckBox->setChecked(prefixSettings->wow64Enabled());
    }

    m_mangohudCheckBox->setEnabled(prefixExists);
    m_obsVkCaptureCheckBox->setEnabled(prefixExists);
    m_xaliaCheckBox->setEnabled(prefixExists);
    m_waylandCheckBox->setEnabled(prefixExists);
    m_useSteamCheckBox->setEnabled(prefixExists);
    m_wow64CheckBox->setEnabled(prefixExists);
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

void MainWindow::onProcessError(const QString& errorText)
{
    QMessageBox::critical(this, tr("Process error"), errorText);
}

void MainWindow::onRunningChanged(bool isRunning)
{
    m_exeSelectionButton->setDisabled(isRunning);
    m_playStopButton->setIcon(isRunning ? QIcon::fromTheme("media-playback-stop") : QIcon::fromTheme("media-playback-start"));
    m_playStopButton->setText(isRunning ? tr("Stop") : tr("Start"));
    m_createExeShortcutButton->setDisabled(isRunning);
    setHidden(isRunning);
}
