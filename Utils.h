#pragma once
#include <cinttypes>
#include <functional>
class Shot;

struct Vector2D {
	double x;
	double y;

	Vector2D operator+(const Vector2D& v) const {
		return { x + v.x, y + v.y };
	}

	Vector2D operator-(const Vector2D& v) const {
		return { x - v.x, y - v.y };
	}

	bool operator==(const Vector2D& v) const {
		return x == v.x && y == v.y;
	}
};

struct TracePoint {
	Vector2D point;
	double time;
};

struct TargetCircle {
	Vector2D center;
	double radius;
};

struct TraceCircle {
	TargetCircle circle;
	uint64_t time;
};

struct ShootController {
    std::function<void()> removePreviousCalibCircle;
    std::function<void(bool)> clearTrace;
    std::function<void(Shot*)> updateView;
    std::function<void(Vector2D)> addToBeforeShotTrace;
    std::function<void(Vector2D)> drawShotCircle;
    std::function<void(Vector2D)> addToAfterShotTrace;
};

enum ANGLE {
	TOP,
	TOP_RIGHT,
	RIGHT,
	BOTTOM_RIGHT,
	BOTTOM,
	BOTTOM_LEFT,
	LEFT,
	TOP_LEFT
};

#define D2P(p1, p2) sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))

//radius of pistol black circle in mm
#define PISTOL_CIRCLE_SIZE 29.75

//radius of pellet in mm
#define PELLET_SIZE 2.25

//length/width of target in mm
#define TARGET_SIZE 170.0
#define TARGET_CENTER (TARGET_SIZE / 2) 
#define TARGET_CENTER_PT Vector2D{ 0.0, 0.0 }

static int CAMERA_INDEX = 1;

static float TRIGGER_DB = 65.0;
#define SILENCE_DURATION 5

#define FPS 120
