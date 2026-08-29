#include "prefix_components_dialog.hpp"

#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixComponentsDialog::PrefixComponentsDialog(const Prefix* prefix, QWidget* parent)
    : QDialog(parent)
    , m_prefix(prefix)
    , m_componentsListProcess(new QProcess(this))
    , m_installedListProcess(new QProcess(this))
    , m_installProcess(new QProcess(this))
    , m_categoryList(new QComboBox(this))
    , m_componentsListWidget(new QListWidget(this))
    , m_searchLineEdit(new QLineEdit(this))
    , m_progressBar(new QProgressBar(this))
    , m_installButton(new QPushButton(QIcon::fromTheme("browser-download"), tr("Install selected"), this))
    , m_closeButton(new QPushButton(QIcon::fromTheme("window-close"), tr("Close"), this))
{
    setWindowTitle(tr("Kisel — Prefix Components"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    auto* layout = new QVBoxLayout(this);

    auto* prefixTitleLabel = new QLabel(tr("<h3>Prefix \"%1\"</h3>").arg(prefix->name()));
    layout->addWidget(prefixTitleLabel);

    auto* titleLabel = new QLabel(tr("Available components for installation:"), this);
    layout->addWidget(titleLabel);

    m_componentsListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_componentsListWidget);

    auto* categoryLabel = new QLabel(tr("Category"), this);
    layout->addWidget(categoryLabel);

    m_categoryList->addItem(tr("dlls"), "dlls"_L1);
    m_categoryList->addItem(tr("fonts"), "fonts"_L1);
    connect(m_categoryList, &QComboBox::currentIndexChanged, this, [this](int index) { loadComponents(); });
    layout->addWidget(m_categoryList);

    m_searchLineEdit->setPlaceholderText(tr("Search by name"));
    layout->addWidget(m_searchLineEdit);

    m_progressBar->setRange(0, 0);
    m_progressBar->hide();
    layout->addWidget(m_progressBar);

    auto* bottomWidget = new QWidget(this);
    layout->addWidget(bottomWidget);

    auto* bottomLayout = new QHBoxLayout(bottomWidget);

    bottomLayout->addWidget(m_installButton);

    bottomLayout->addWidget(m_closeButton);

    connect(m_searchLineEdit, &QLineEdit::textChanged, this, &PrefixComponentsDialog::filterItems);
    connect(m_installButton, &QPushButton::clicked, this, &PrefixComponentsDialog::onInstallCancelButtonClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &PrefixComponentsDialog::close);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("UMU_RUNTIME_UPDATE"_L1, APP_SETTINGS->runtimeAutoUpdate() ? "1"_L1 : "0"_L1);
    env.insert("WINEPREFIX"_L1, m_prefix->path());
    env.insert("PROTONPATH"_L1, m_prefix->settings()->ctPath());

    // Don't use pure winetricks!

    m_componentsListProcess->setProcessEnvironment(env);
    m_componentsListProcess->setProgram(APP_SETTINGS->umuPath());
    connect(m_componentsListProcess, &QProcess::finished, this, &PrefixComponentsDialog::onComponentsListLoaded);

    m_installedListProcess->setProcessEnvironment(env);
    m_installedListProcess->setProgram(APP_SETTINGS->umuPath());
    m_installedListProcess->setArguments({ "winetricks"_L1, "list-installed"_L1 });
    connect(m_installedListProcess, &QProcess::finished, this, &PrefixComponentsDialog::onInstalledListLoaded);

    m_installProcess->setProcessEnvironment(env);
    m_installProcess->setProgram(APP_SETTINGS->umuPath());
    connect(m_installProcess, &QProcess::finished, this, &PrefixComponentsDialog::onInstallFinished);

    loadComponents();
}

void PrefixComponentsDialog::loadComponents()
{
    m_componentsListWidget->clear();
    m_componentsListWidget->setEnabled(false);
    m_categoryList->setEnabled(false);
    m_installButton->setEnabled(false);
    m_searchLineEdit->setEnabled(false);
    m_progressBar->show();

    m_componentsListProcess->setArguments({ "winetricks"_L1, m_categoryList->currentData().toString(), "list"_L1 });
    m_componentsListProcess->start();
}

void PrefixComponentsDialog::onComponentsListLoaded(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        resetWidgetsState();
        QMessageBox::critical(this, tr("Update error"),
            tr("Failed to get list of components available for installation: %1").arg(m_componentsListProcess->errorString()));
        return;
    }

    QString output = QString::fromUtf8(m_componentsListProcess->readAllStandardOutput());
    const QStringList& lines = output.split(u'\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        parseAndAddLine(line.trimmed());
    }

    m_installedListProcess->start();
}

void PrefixComponentsDialog::onInstalledListLoaded(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        resetWidgetsState();
        QMessageBox::critical(this, tr("Update error"),
            tr("Failed to get list of installed components: %1").arg(m_installedListProcess->errorString()));
        return;
    }

    QString output = QString::fromUtf8(m_installedListProcess->readAllStandardOutput());
    QStringList lines = output.split(u'\n', Qt::SkipEmptyParts);

    for (int i = 0; i < m_componentsListWidget->count(); ++i) {
        QListWidgetItem* item = m_componentsListWidget->item(i);
        if (lines.contains(item->text())) {
            item->setCheckState(Qt::Checked);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }
    }

    resetWidgetsState();
    m_installButton->setEnabled(true);
}

void PrefixComponentsDialog::resetWidgetsState()
{
    m_componentsListWidget->setEnabled(true);
    m_categoryList->setEnabled(true);
    m_searchLineEdit->setEnabled(true);
    m_installButton->setText(tr("Install selected"));
    m_installButton->setIcon(QIcon::fromTheme("browser-download"));
    m_progressBar->hide();
}

void PrefixComponentsDialog::parseAndAddLine(const QString& line)
{
    if (line.isEmpty() || line.startsWith("=="_L1)) {
        return;
    }

    static QRegularExpression re(R"(^([^\s]+)\s+(.*)$)"_L1);
    QRegularExpressionMatch match = re.match(line);

    if (!match.hasMatch()) {
        return;
    }

    const QString& verb = match.captured(1);
    const QString& description = match.captured(2).trimmed();

    auto* item = new QListWidgetItem(verb, m_componentsListWidget);

    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    item->setToolTip(tr("<b>Description:</b> %1").arg(description));
}

void PrefixComponentsDialog::onInstallCancelButtonClicked()
{
    if (m_installProcess->state() == QProcess::Running) {
        auto answer = QMessageBox::question(this, tr("Confirmation"), tr("Cancel the installation process?"));
        if (answer == QMessageBox::Yes) {
            cancelInstallation();
            return;
        }
    }

    updateSelectedComponents();

    if (m_selectedComponents.isEmpty()) {
        QMessageBox::information(this, tr("There is nothing to install"), tr("Mark the components to install in the prefix"));
        return;
    }

    auto answer = QMessageBox::question(this, tr("Confirmation"),
        tr("Install selected components?\n%1").arg(m_selectedComponents.join("\n")));
    if (answer == QMessageBox::Yes) {
        installSelected();
    }
}

void PrefixComponentsDialog::updateSelectedComponents()
{
    m_selectedComponents.clear();
    for (int i = 0; i < m_componentsListWidget->count(); ++i) {
        QListWidgetItem* item = m_componentsListWidget->item(i);
        if (item->checkState() == Qt::Checked && item->flags().testFlag(Qt::ItemIsEnabled)) {
            m_selectedComponents << item->text();
        }
    }
}

void PrefixComponentsDialog::installSelected()
{
    m_componentsListWidget->setEnabled(false);
    m_categoryList->setEnabled(false);
    m_searchLineEdit->setEnabled(false);
    m_installButton->setText(tr("Stop"));
    m_installButton->setIcon(QIcon::fromTheme("media-playback-stop"));
    m_progressBar->show();

    m_installProcess->setArguments(QStringList() << "winetricks"_L1 << "-q"_L1 << m_selectedComponents); // Don't use pure winetricks!
    m_installProcess->start();
}

void PrefixComponentsDialog::onInstallFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    resetWidgetsState();
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QMessageBox::critical(this, tr("Installation error"),
            tr("Failed to install the selected components: %1").arg(m_installProcess->errorString()));
    } else {
        for (int i = 0; i < m_componentsListWidget->count(); ++i) {
            QListWidgetItem* item = m_componentsListWidget->item(i);
            if (m_selectedComponents.contains(item->text())) {
                item->setCheckState(Qt::Checked);
                item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            }
        }

        QMessageBox::information(this, tr("Completed"), tr("Successfully installed!"));
    }
}

void PrefixComponentsDialog::closeEvent(QCloseEvent* event)
{
    if (m_componentsListProcess->state() == QProcess::Running) {
        m_componentsListProcess->terminate();
        if (!m_componentsListProcess->waitForFinished()) {
            m_componentsListProcess->kill();
        }
    }

    if (m_installedListProcess->state() == QProcess::Running) {
        m_installedListProcess->terminate();
        if (!m_installedListProcess->waitForFinished()) {
            m_installedListProcess->kill();
        }
    }

    if (m_installProcess->state() == QProcess::Running) {
        auto answer = QMessageBox::question(this, tr("Confirmation"), tr("Cancel the installation process and close the window?"));
        if (answer == QMessageBox::Yes) {
            cancelInstallation();
        } else {
            event->ignore();
            return;
        }
    }

    event->accept();
    QDialog::closeEvent(event);
}

void PrefixComponentsDialog::cancelInstallation()
{
    if (m_installProcess->state() == QProcess::Running) {
        m_installProcess->terminate();
        if (!m_installProcess->waitForFinished()) {
            m_installProcess->kill();
        }
    }
}

void PrefixComponentsDialog::filterItems(const QString& text)
{
    for (int i = 0; i < m_componentsListWidget->count(); ++i) {
        QListWidgetItem* item = m_componentsListWidget->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}