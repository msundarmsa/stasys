#pragma once

#include "Utils.h"
#include <vector>

class ShotTrace {
    private:
        std::vector<TracePoint> beforeShotTrace;
        TracePoint shotPoint = {{0, 0}, -1, -1};
        std::vector<TracePoint> afterShotTrace;
    public:
        void reset();
        void addTracePoint(TracePoint tp);
        bool afterShot();
        bool isShotPointSet();
        void setTriggerPulled();
        void setShotPoint(TracePoint tp);
        std::vector<TracePoint> getCurrentShotTrace();
        std::vector<TracePoint> getBeforeShotTrace();
        TracePoint getShotPoint();
        std::vector<TracePoint> getAfterShotTrace();
};
