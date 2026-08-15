#include "ct.hpp"

using namespace kisel;

Ct::Ct(const QString& path, QObject* parent)
    : QObject(parent)
    , m_dir(path)
    , m_status(m_dir.exists() ? Installed : Unknown)
    , m_progress(0)
{
    if (m_dir.absolutePath().contains(QStringLiteral("steam"))) {
        m_icon = QIcon::fromTheme("steam");
    } else {
        m_icon = QIcon(":/icons/kisel.svg");
    }
}

QString Ct::path() const
{
    return m_dir.absolutePath();
};

QDir Ct::dir() const
{
    return m_dir;
}

QString Ct::name() const
{
    return m_dir.dirName();
};

QIcon Ct::icon() const
{
    return m_icon;
};

void Ct::setStatus(Status status)
{
    m_status = status;
}

Ct::Status Ct::status() const
{
    return m_status;
}

void Ct::setProgress(int progress)
{
    m_progress = progress;
}

int Ct::progress() const
{
    return m_progress;
}

bool Ct::exists() const
{
    return m_dir.exists();
}

QString Ct::statusToString(Status status)
{
    static QHash<Status, QString> statusMap {
        { Downloading, tr("Downloading") },
        { Unpacking, tr("Unpacking") },
        { Installed, tr("Installed") },
        { Unknown, tr("Unknown") }
    };
    if (statusMap.contains(status)) {
        return statusMap.value(status);
    }
    return { };
}