#pragma once

#include "SamplingTasksRegistrar.h"
#include "ActiveObject.h"
#include "BrewControl.h"

typedef SamplingTasksRegistrar< void (DataActiveObject< BrewControl > &) > BrewControlSamplingTasks;
