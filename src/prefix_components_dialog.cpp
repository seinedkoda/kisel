#include "prefix_components_dialog.hpp"

#include <QCloseEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSizeGrip>
#include <QVBoxLayout>

#include "app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixComponentsDialog::PrefixComponentsDialog(const Prefix& prefix, QWidget* parent)
    : QDialog(parent)
    , m_prefix(prefix)
    , m_updateProcess(new QProcess(this))
    , m_installProcess(new QProcess(this))
    , m_categoryList(new QComboBox(this))
    , m_componentsListWidget(new QListWidget(this))
    , m_searchLineEdit(new QLineEdit(this))
    , m_progressBar(new QProgressBar(this))
    , m_installButton(new QPushButton(QIcon::fromTheme("browser-download"), tr("Install selected"), this))
    , m_closeButton(new QPushButton(tr("Close"), this))
{
    setWindowTitle(tr("Kisel — Prefix Components"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    auto* layout = new QVBoxLayout(this);

    auto* prefixTitleLabel = new QLabel(tr("<h3>Prefix \"%1\"</h3>").arg(prefix.name()));
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
    connect(m_updateProcess, &QProcess::finished, this, &PrefixComponentsDialog::onUpdateFinished);
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

    m_updateProcess->start(APP_SETTINGS->winetricksPath(), { m_categoryList->currentData().toString(), "list"_L1 });
}

void PrefixComponentsDialog::onUpdateFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_componentsListWidget->setEnabled(true);
    m_categoryList->setEnabled(true);
    m_installButton->setEnabled(true);
    m_searchLineEdit->setEnabled(true);
    m_progressBar->hide();

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QMessageBox::critical(this, tr("Update error"), tr("Unable to get list of components to install using winetricks"));
        return;
    }

    QString output = QString::fromUtf8(m_updateProcess->readAllStandardOutput());
    const QStringList& lines = output.split(u'\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        parseAndAddLine(line.trimmed());
    }
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

    auto answer = QMessageBox::question(this, tr("Confirmation"), tr("Install selected components?"));
    if (answer == QMessageBox::Yes) {
        installSelected();
    }
}

QStringList PrefixComponentsDialog::selectedComponents() const
{
    QStringList selectedItems;
    for (int i = 0; i < m_componentsListWidget->count(); ++i) {
        QListWidgetItem* item = m_componentsListWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            selectedItems << item->text();
        }
    }
    return selectedItems;
}

void PrefixComponentsDialog::installSelected()
{
    const QStringList& selectedItems = selectedComponents();

    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, tr("There is nothing to install"), tr("Mark the components to install in the prefix"));
        return;
    }

    m_componentsListWidget->setEnabled(false);
    m_categoryList->setEnabled(false);
    m_searchLineEdit->setEnabled(false);
    m_installButton->setText(tr("Stop"));
    m_installButton->setIcon(QIcon::fromTheme("stop"));
    m_progressBar->show();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("WINEPREFIX"_L1, m_prefix.path());
    m_installProcess->setProcessEnvironment(env);

    m_installProcess->start(APP_SETTINGS->winetricksPath(), QStringList() << "-q"_L1 << selectedItems);
}

void PrefixComponentsDialog::onInstallFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_componentsListWidget->setEnabled(true);
    m_categoryList->setEnabled(true);
    m_searchLineEdit->setEnabled(true);
    m_installButton->setText(tr("Install selected"));
    m_installButton->setIcon(QIcon::fromTheme("browser-download"));
    m_progressBar->hide();

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QMessageBox::critical(this, tr("Installation error"), tr("Failed to install the selected components using winetricks"));
    } else {
        QMessageBox::information(this, tr("Completed"), tr("Successfully installed!"));
    }
}

void PrefixComponentsDialog::closeEvent(QCloseEvent* event)
{
    if (m_updateProcess->state() == QProcess::Running) {
        m_updateProcess->terminate();
        if (!m_updateProcess->waitForFinished()) {
            m_updateProcess->kill();
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