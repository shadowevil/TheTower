#include "Projectile.h"
#include "allegro5/allegro5.h"

void Projectile::InitProjectile(float px, float py, double time, float eCX, float eCY)
{
	float _dirX = 0.0f;
	float _dirY = 0.0f;
	getDirection(px, py, eCX, eCY, &_dirX, &_dirY);

	projX = px;
	projY = py;
	projectileTimer = time;
	dirX = _dirX;
	dirY = _dirY;
}

void Projectile::CheckForDeletion()
{
	if (projX >= 800 || projY >= 600) readyToDelete = true;
	if (projX <= 0 || projY <= 0) readyToDelete = true;
}

void Projectile::moveProjectile()
{
	if (isExploding == false && isExploded == false)
	{
		projX -= dirX * projectileSpeed;
		projY -= dirY * projectileSpeed;

		lastPositions.push_back({ projX, projY });
	}
}

Projectile::lp Projectile::getLaser(int laserLength)
{
	std::vector<vec2d>::iterator begin = lastPositions.begin();
	std::vector<vec2d>::iterator end = lastPositions.end() - 1;
	if (lastPositions.size() > (size_t)laserLength)
	{
		begin = lastPositions.end() - 1 - laserLength;
		end = lastPositions.end() - 1;
	}

	lp p;
	p.begin = (*begin);
	p.end = (*end);

	// Maintain a small vector
	if ((int)(lastPositions.size() - 1) >= laserLength) lastPositions.erase(begin);
	return p;
}

bool Projectile::CheckCollision(float eCX, float eCY, float eSZ)
{
	if (projX >= eCX - (eSZ / 2) && projX <= eCX + (eSZ / 2) &&
		projY >= eCY - (eSZ / 2) && projY <= eCY + (eSZ / 2))
	{
		readyToDelete = true;
		return true;
	}
	return false;
}

void Projectile::Explode(double time)
{
	isExploding = true;
	for (int i = 0; i < 20; i++)
	{
		float vel = getRandom(-2.5f, 2.5f);
		float vel1 = getRandom(-2.5f, 2.5f);
		particles.push_back({ projX, projY, 0.005, vel, vel1, time });
	}
}

void Projectile::checkExplosion()
{
	if (isExploding)
	{
		for (auto& _p : particles)
		{
			if (al_get_time() - _p.timer >= _p.updateFrequency)
			{
				_p.timer = al_get_time();
				_p.x += _p.xVel;
				_p.y += _p.yVel;

				if (_p.xVel > 0.0f)
				{
					_p.xVel -= 0.05f;
					if (_p.xVel <= 0.0f) _p.xVel = 0.0f;
				}
				else {
					_p.xVel += 0.05f;
					if (_p.xVel >= 0.0f) _p.xVel = 0.0f;
				}

				if (_p.yVel > 0.0f)
				{
					_p.yVel -= 0.05f;
					if (_p.yVel <= 0.0f) _p.yVel = 0.0f;
				}
				else {
					_p.yVel += 0.05f;
					if (_p.yVel >= 0.0f) _p.yVel = 0.0f;
				}

				_p.alpha -= 7.5f;
				if (_p.alpha <= 0.0f) _p.alpha = 0.0f;
			}
		}
	}
}

void Projectile::checkLaserParticles()
{
	if (!isExploding)
	{
		for (auto& _p : shootParticles)
		{
			if (al_get_time() - _p.timer >= _p.updateFrequency)
			{
				_p.timer = al_get_time();
				_p.x += _p.xVel;
				_p.y += _p.yVel;

				if (_p.xVel > 0.0f)
				{
					_p.xVel -= 0.05f;
					if (_p.xVel <= 0.0f) _p.xVel = 0.0f;
				}
				else {
					_p.xVel += 0.05f;
					if (_p.xVel >= 0.0f) _p.xVel = 0.0f;
				}

				if (_p.yVel > 0.0f)
				{
					_p.yVel -= 0.05f;
					if (_p.yVel <= 0.0f) _p.yVel = 0.0f;
				}
				else {
					_p.yVel += 0.05f;
					if (_p.yVel >= 0.0f) _p.yVel = 0.0f;
				}

				_p.alpha -= 7.5f;
				if (_p.alpha <= 0.0f) _p.alpha = 0.0f;
			}
		}
	}
}