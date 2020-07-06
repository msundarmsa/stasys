#pragma once

#include "Utils.h"
#include "ShotTrace.h"
#include <math.h>

class Shot
{
public:
	Shot(int sn, double score, double angle_val, double stab, double desc, double aim);
	Shot(int sn);
	Shot(int sn, ShotTrace shotTrace);
	Shot(int sn, TracePoint shotPoint);
	Shot(TracePoint shotPoint);

private:
	ShotTrace shotTrace;

	int sn;
	double score;
	double angle_val = -1;
	enum ANGLE angle;
	double stab;
	double desc;
	double aim;

public:
	double getScore();
	enum ANGLE getAngle();
	double getAngleVal();
	double getStab();
	double getDesc();
	double getAim();
	int getSn();
	TracePoint getShotPoint();
	void clearShot();

private:
	void calcStabDescAim();
	void calcScore();
	void calcAngle();
};
