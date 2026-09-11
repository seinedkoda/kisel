#include "app.hpp"

#include <QApplicationStatic>

using namespace kisel;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Q_APPLICATION_STATIC(App, g_app)

App::App(QObject* parent)
    : QObject(parent)
    , m_runManager(new RunManager(this))
    , m_prefixModel(new PrefixModel(this))
    , m_ctModel(new CtModel(this))
    , m_ctInstaller(new CtInstaller(this))
    , m_shortcutModel(new ShortcutModel(this))
{
}

App* App::instance()
{
    return g_app();
}

RunManager* App::runManager()
{
    return m_runManager;
}

PrefixModel* App::prefixModel()
{
    return m_prefixModel;
}

CtModel* App::ctModel()
{
    return m_ctModel;
}

CtInstaller* App::ctInstaller()
{
    return m_ctInstaller;
}

ShortcutModel* App::shortcutModel()
{
    return m_shortcutModel;
}
