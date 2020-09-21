#include "ShotTrace.h"

void ShotTrace::reset(){
    beforeShotTrace.clear();
    afterShotTrace.clear();
    shotPoint = {{ 0, 0 }, -1, -1};
}

void ShotTrace::addTracePoint(TracePoint tp) {
    if (shotPoint.time < 0) {
      beforeShotTrace.push_back(tp);
    } else {
      afterShotTrace.push_back(tp);
    }
}

void ShotTrace::setTriggerPulled() {
    shotPoint.time = 0;
}

bool ShotTrace::afterShot() {
    return shotPoint.time >= 0;
}

bool ShotTrace::isShotPointSet() {
    return shotPoint.time > 0;
}

std::vector<TracePoint> ShotTrace::getCurrentShotTrace() {
    if (afterShot()) {
      return afterShotTrace;
    } else{
      return beforeShotTrace;
    }
}

std::vector<TracePoint> ShotTrace::getBeforeShotTrace() {
    return beforeShotTrace;
}

std::vector<TracePoint> ShotTrace::getAfterShotTrace() {
    return afterShotTrace;
}

TracePoint ShotTrace::getShotPoint() {
    return shotPoint;
}

void ShotTrace::setShotPoint(TracePoint shotPoint) {
    this->shotPoint = shotPoint;
}
