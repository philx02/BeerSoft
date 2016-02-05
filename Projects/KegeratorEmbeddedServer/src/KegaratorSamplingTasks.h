#pragma once

#include "SamplingTasksRegistrar.h"
#include "ActiveObject.h"
#include "KegaratorMetrics.h"

typedef SamplingTasksRegistrar< void (DataActiveObject< KegaratorMetrics > &) > KegaratorSamplingTasks;
