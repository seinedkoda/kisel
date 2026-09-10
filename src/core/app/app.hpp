#pragma once

#include <QObject>

#include "core/compatibilitytools/ct_model.hpp"
#include "core/prefix/prefix_model.hpp"
#include "core/run/run_manager.hpp"
#include "core/shortcuts/shortcut_model.hpp"

namespace kisel {
#define RUN_MANAGER App::instance()->runManager()
#define PREFIX_MODEL App::instance()->prefixModel()
#define CT_MODEL App::instance()->ctModel()
#define SHORTCUT_MODEL App::instance()->shortcutModel()

class App : public QObject {
public:
    App(QObject* parent = nullptr);
    static App* instance();

    RunManager* runManager();
    PrefixModel* prefixModel();
    CtModel* ctModel();
    ShortcutModel* shortcutModel();

private:
    RunManager* m_runManager;
    PrefixModel* m_prefixModel;
    CtModel* m_ctModel;
    ShortcutModel* m_shortcutModel;
};
}