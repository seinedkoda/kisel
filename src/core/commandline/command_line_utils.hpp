#pragma once

#include "core/run/run_config.hpp"

namespace kisel {

void parseCommandLine(const QStringList& args, RunConfig* config);

}