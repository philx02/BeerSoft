#pragma once

#include "InterruptTasksRegistrar.h"
#include "ActiveObject.h"
#include "KegaratorMetrics.h"

typedef InterruptTasksRegistrar< void (DataActiveObject< KegaratorMetrics > &) > KegaratorInterruptTasks;
