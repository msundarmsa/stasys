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

    Vector2D operator/(const double f) const {
        return { x / f, y / f };
    }

    Vector2D operator*(const double f) const {
        return { x * f, y * f };
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

inline const char* toString(ANGLE angle)
{
    switch (angle)
    {
        case TOP: return "top";
        case TOP_RIGHT: return "top_right";
        case RIGHT: return "right";
        case BOTTOM_RIGHT: return "bottom_right";
        case BOTTOM: return "bottom";
        case BOTTOM_LEFT: return "bottom_left";
        case LEFT: return "left";
        case TOP_LEFT: return "top_left";
    }
}

#define D2P(p1, p2) sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))

//radius of pistol black circle in mm
#define PISTOL_CIRCLE_SIZE 29.75

//radius of pellet in mm
#define PELLET_SIZE 2.25

//length/width of target in mm
#define TARGET_SIZE 170.0
#define TARGET_CENTER (TARGET_SIZE / 2) 
#define TARGET_CENTER_PT Vector2D{ 0.0, 0.0 }

#define SILENCE_DURATION 5000
#define FPS 120

#define VELOCITY_FACTOR 17.34
#define TIME_OFFSET 1.349
