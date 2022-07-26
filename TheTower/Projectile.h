#pragma once
#include "Helper.h"

class Projectile
{
public:
	// Projectile Particles
	struct pp
	{
		float x = 0.0f;
		float y = 0.0f;
		double updateFrequency = 0.005;
		float xVel = 0.0f;
		float yVel = 0.0f;
		double timer;
		float alpha = 255.0f;
	};
	std::vector<pp> particles;
	std::vector<pp> shootParticles;

	struct lp
	{
		vec2d begin;
		vec2d end;
	};

	float projX = 0.0f;
	float projY = 0.0f;
	double projectileTimer = 0.0;
	double projectileDelay = 0.1;
	bool readyToDelete = false;
	float dirX = 0.0f;
	float dirY = 0.0f;
	float projectileSpeed = 5.0f;
	std::vector<vec2d> lastPositions;

	bool isExploding = false;
	bool isExploded = false;

	void InitProjectile(float px, float py, double time, float dirx, float diry);
	void CheckForDeletion();
	void moveProjectile();
	bool CheckCollision(float eCX, float eCY, float eSZ);
	lp getLaser(int laserLength);
	void checkLaserParticles();
	void checkExplosion();
	void Explode(double time);
};

