#include "prefix_components_dialog.hpp"

#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "core/app/app.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixComponentsDialog::PrefixComponentsDialog(Prefix* prefix, QWidget* parent)
    : QDialog(parent)
    , m_prefix(prefix)
    , m_categoryList(new QComboBox(this))
    , m_componentsListWidget(new QListWidget(this))
    , m_searchLineEdit(new QLineEdit(this))
    , m_progressBar(new QProgressBar(this))
    , m_installButton(new QPushButton(QIcon::fromTheme("browser-download"), tr("Install selected"), this))
    , m_closeButton(new QPushButton(QIcon::fromTheme("window-close"), tr("Close"), this))
    , m_installationCancelled(false)
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

    RUN_MANAGER->runComponentsList(m_prefix, m_categoryList->currentData().toString(),
        [this](int exitCode, QProcess::ExitStatus exitStatus, const QString& output) {
            if (exitStatus == QProcess::CrashExit) {
                resetWidgetsState();
                QMessageBox::critical(this, tr("Update error"),
                    tr("Failed to get list of components available for installation, exit code: %1").arg(exitCode));
                return;
            }

            const QStringList& lines = output.split(u'\n', Qt::SkipEmptyParts);

            for (const QString& line : lines) {
                parseAndAddLine(line.trimmed());
            }

            loadInstalledComponents();
        });
}

void PrefixComponentsDialog::loadInstalledComponents()
{
    RUN_MANAGER->runInstalledComponentsList(m_prefix, m_categoryList->currentData().toString(),
        [this](int exitCode, QProcess::ExitStatus exitStatus, const QString& output) {
            if (exitStatus != QProcess::NormalExit || exitCode != 0) {
                resetWidgetsState();
                QMessageBox::critical(this, tr("Update error"),
                    tr("Failed to get list of installed components, exit code: %1").arg(exitCode));
                return;
            }

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
        });
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
    if (RUN_MANAGER->isRunning() && RUN_MANAGER->taskName() == tr("Installing components")) {
        auto answer = QMessageBox::question(this, tr("Confirmation"), tr("Cancel the installation process?"));
        if (answer == QMessageBox::Yes) {
            m_installationCancelled = true;
            RUN_MANAGER->stop();
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

    RUN_MANAGER->runComponentsInstallation(m_prefix, m_selectedComponents,
        [this](int exitCode, QProcess::ExitStatus exitStatus, const QString& output) {
            resetWidgetsState();

            if (m_installationCancelled) {
                m_installationCancelled = false;
                QMessageBox::information(this, tr("Completed"), tr("Installation cancelled"));
            } else if (exitStatus != QProcess::NormalExit || exitCode != 0) {
                QMessageBox::critical(this, tr("Installation error"),
                    tr("Failed to install the selected components, exit code: %1").arg(exitCode));
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
        });
}

void PrefixComponentsDialog::closeEvent(QCloseEvent* event)
{
    if (RUN_MANAGER->isRunning() && RUN_MANAGER->taskName() == tr("Installing components")) {
        auto answer = QMessageBox::question(this, tr("Confirmation"), tr("Cancel the installation process and close the window?"));
        if (answer == QMessageBox::Yes) {
            m_installationCancelled = true;
        } else {
            event->ignore();
            return;
        }
    }

    RUN_MANAGER->stop();

    event->accept();
    QDialog::closeEvent(event);
}

void PrefixComponentsDialog::filterItems(const QString& text)
{
    for (int i = 0; i < m_componentsListWidget->count(); ++i) {
        QListWidgetItem* item = m_componentsListWidget->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}