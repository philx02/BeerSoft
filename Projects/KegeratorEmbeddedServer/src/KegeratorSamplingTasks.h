#pragma once

#include "SamplingTasksRegistrar.h"
#include "ActiveObject.h"
#include "KegeratorMetrics.h"

typedef SamplingTasksRegistrar< void (DataActiveObject< KegeratorMetrics > &) > KegeratorSamplingTasks;
