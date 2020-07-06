#pragma once

#include "Shot.h"
#include "Utils.h"

class ShootController {
	public:
		virtual void removePreviousCalibCircle() {};
		virtual void clearTrace(bool) {};
		virtual void updateView(Shot*) {};
		virtual void addToBeforeShotTrace(Vector2D) {};
		virtual void addToAfterShotTrace(Vector2D) {};
		virtual void drawShotCircle(Vector2D) {};
};
