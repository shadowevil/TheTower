#pragma once
#include <iostream>
#include <vector>

#define MAX_LEVEL		1000 + 1
#define NUM_FONTS		255

#ifndef HELPER
#define HELPER

extern float getRandom(float min, float max);
extern int getRandom(int min, int max);
extern void getDirection(float px, float py, float ex, float ey, float* dirX, float* dirY);
extern float getDistance(float px, float py, float ex, float ey);

class vec2d
{
public:
	float x;
	float y;
};

class polygon
{
public:
	std::vector<vec2d> p;
	vec2d pos = { 0, 0 };
	float angle = 0.0f;
	std::vector<vec2d> o;
	bool overlap = false;
};


extern bool ShapeOverlap_SAT(polygon &r1, polygon &r2);

class MouseState
{
public:
	int x = 0;
	int y = 0;
	bool isLeftButtonDown = false;
	bool isRightButtonDown = false;
	bool didClick = false;
};


extern MouseState mouseState;
extern float w_Width;
extern float w_Height;

class statusText
{
public:
	double textFadeTimer = 0.0;
	double textFadeDelay = 0.05;
	float textFadeAlpha = 255.0f;

	double textMoveTimer = 0.0;
	double textMoveDelay = 0.05;
	float textMoveOffsetY = 0.0f;

	bool readyToDelete = false;

	std::string* str = nullptr;
	int strCount = 0;

	float x = 0.0f;
	float y = 0.0f;
};
#endif