#include "Shot.h"
#include <vector>

Shot::Shot(int sn, double score, double angle_val, double stab, double desc, double aim)
{
	// set object parameters
	this->sn = sn;
	this->score = score;
	this->angle_val = angle_val;
	this->stab = stab;
	this->desc = desc;
    this->aim = aim;

    this->shotTrace.setShotPoint({{0, 0}, 0});

	// convert angle value (0 to 2pi) to enum ANGLE
	calcAngle();
}

Shot::Shot(int sn)
{
	this->sn = sn;
}

Shot::Shot(int sn, ShotTrace shotTrace)
{
	this->sn = sn;
	this->shotTrace = shotTrace;

	calcScore();
	calcAngle();
	calcStabDescAim();
}

Shot::Shot(int sn, TracePoint shotPoint)
{
	this->sn = sn;
	this->shotTrace.setShotPoint(shotPoint);
	calcAngle();
}

Shot::Shot(TracePoint shotPoint)
{
	this->shotTrace.setShotPoint(shotPoint);
	calcAngle();
}

double Shot::getScore()
{
	return score;
}

enum ANGLE Shot::getAngle()
{
	return angle;
}

double Shot::getAngleVal()
{
	return angle_val;
}

double Shot::getStab()
{
	return stab;
}

double Shot::getDesc()
{
	return desc;
}

double Shot::getAim()
{
	return aim;
}

int Shot::getSn()
{
	return sn;
}

ShotTrace Shot::getShotTrace()
{
    return shotTrace;
}

TracePoint Shot::getShotPoint()
{
	return shotTrace.getShotPoint();
}

void Shot::clearShot()
{
	score = 0;
	angle = TOP;
	angle_val = 0;
	stab = 0;
	desc = 0;
	aim = 0;

	shotTrace.reset();
}

void Shot::calcStabDescAim()
{
    int entered10RingIdx = -1;
    double entered10RingTime = -1;
    int num10Ring = 0;
	std::vector<TracePoint> beforeShotTrace = shotTrace.getBeforeShotTrace();
    double firstFrameTime = beforeShotTrace[0].time;
    double lastFrameTime = beforeShotTrace.back().time;

    for (size_t i = 0; i < beforeShotTrace.size(); i++)
	{
        TracePoint currTP = beforeShotTrace[i];
		double currDist = D2P(currTP.point, TARGET_CENTER_PT);

        if (currDist <= 8.0)
		{
            if (entered10RingIdx == -1)
			{
                entered10RingIdx = i;
                entered10RingTime = currTP.time;
			}

            num10Ring++;
		}
	}

    if (entered10RingIdx != -1)
	{
        double total10Ring = beforeShotTrace.size() - entered10RingIdx + 1.0;
        stab = (num10Ring / (total10Ring)) * 100.0;
        desc = (entered10RingTime - firstFrameTime) / 1000;
        aim = (lastFrameTime - entered10RingTime) / 1000;
	}
	else
	{
		stab = 0;
		desc = 0;
		aim = 0;
	}
}

void Shot::calcScore()
{
	TracePoint shotPoint = shotTrace.getShotPoint();
	double myScore = 11 - D2P(shotPoint.point, TARGET_CENTER_PT) / 8.0;
    score = myScore > 10.9 ? 10.9 : myScore < 0.0 ? 0.0 : myScore;
}

void Shot::calcAngle()
{
	TracePoint shotPoint = shotTrace.getShotPoint();

	angle_val = atan2(shotPoint.point.y, shotPoint.point.x);

	if (shotPoint.point.x < 0 && shotPoint.point.y >= 0) {
		angle_val = 5 * M_PI / 2 - angle_val;
	}
	else {
		angle_val = M_PI / 2 - angle_val;
	}

	if (angle_val <= M_PI / 8 || angle_val >= 15 * M_PI / 8)
	{
		angle = TOP;
	}
	else if (angle_val <= 3 * M_PI / 8 && angle_val >= M_PI / 8)
	{
		angle = TOP_RIGHT;
	}
	else if (angle_val <= 5 * M_PI / 8 && angle_val >= 3 * M_PI / 8)
	{
		angle = RIGHT;
	}
	else if (angle_val <= 7 * M_PI / 8 && angle_val >= 5 * M_PI / 8)
	{
		angle = BOTTOM_RIGHT;
	}
	else if (angle_val <= 9 * M_PI / 8 && angle_val >= 7 * M_PI / 8)
	{
		angle = BOTTOM;
	}
	else if (angle_val <= 11 * M_PI / 8 && angle_val >= 9 * M_PI / 8)
	{
		angle = BOTTOM_LEFT;
	}
	else if (angle_val <= 13 * M_PI / 8 && angle_val >= 11 * M_PI / 8)
	{
		angle = LEFT;
	}
	else
	{
		angle = TOP_LEFT;
	}
}
