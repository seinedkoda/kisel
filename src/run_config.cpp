#include "run_config.hpp"

#include "prefix_model.hpp"

using namespace kisel;

RunConfig::RunConfig(QObject* parent)
    : QObject(parent)
{
}

void RunConfig::setPrefix(Prefix* prefix)
{
    m_prefix = prefix;
}

void RunConfig::setPrefixName(const QString& prefixName)
{
    m_prefix = PREFIX_MODEL->forName(prefixName);
}

Prefix* RunConfig::prefix() const
{
    return m_prefix;
}

void RunConfig::setCt(Ct* ct)
{
    m_ct = ct;
}

Ct* RunConfig::ct() const
{
    return m_ct;
}