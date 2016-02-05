#pragma once

#include "InterruptTasksRegistrar.h"
#include "ActiveObject.h"
#include "KegeratorMetrics.h"

typedef InterruptTasksRegistrar< void (DataActiveObject< KegaratorMetrics > &) > KegaratorInterruptTasks;
