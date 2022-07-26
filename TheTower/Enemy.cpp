#include "Enemy.h"

void Enemy::newEnemy(int playerLevel, float width, float height)
{

	float x = 0.0f;
	float y = 0.0f;

	if (getRandom(0.0f, 1.0f) >= 0.5f) y = getRandom(height, height + 100.0f);
	else							   y = getRandom(-100.0f, 0.0f);
	if (getRandom(0.0f, 1.0f) >= 0.5f) x = getRandom(-100.0f, 0.0f);
	else							   x = getRandom(width, width + 100.0f);

	enemyShape.pos = { x, y };

	enemyMaxHealth = getRandom(15, 25);
	enemyMaxHealth += 10 * (playerLevel + 1);
	float defaultSize = 10.0f + (enemyMaxHealth / 10.0f);
	enemySpeed = defaultSize / 25.0f;
	enemyHealth = enemyMaxHealth;
	damage = defaultSize * 0.4f;

	if (defaultSize >= 75.0f) defaultSize = 75.0f;

	enemyShape.pos = { x, y };
	enemyShape.angle = 0.0f;
	enemyShape.o.push_back({ -defaultSize, -defaultSize });
	enemyShape.o.push_back({ -defaultSize, defaultSize });
	enemyShape.o.push_back({ defaultSize, defaultSize });
	enemyShape.o.push_back({ defaultSize, -defaultSize });
	enemyShape.p.resize(4);

	enemySize = -(enemyShape.o[0].x * 2);
}

void Enemy::moveEnemy(float x, float y, bool colliding)
{
	distance = getDistance(enemyShape.pos.x, enemyShape.pos.y, x, y);

	float dirX = 0.0f;
	float dirY = 0.0f;
	if (colliding)
	{
		enemyShape.angle += 0.05f;
	}
	else {
		getDirection(enemyShape.pos.x, enemyShape.pos.y, x, y, &dirX, &dirY);
		enemyShape.pos.x -= dirX * enemySpeed;
		enemyShape.pos.y -= dirY * enemySpeed;
	}
}

void Enemy::takeDamage(float damage)
{
	statusText* dmg = new statusText;
	dmg->str = new std::string[2]{
		"-", std::to_string((int)damage)
	};
	dmg->strCount = 2;
	dmgText.push_back(dmg);

	enemyHealth -= damage;

	if (enemyHealth <= 0.0f)
	{
		enemyHealth = 0;
		readyToDelete = true;
		Explode(al_get_time());
	}
}

void Enemy::checkExplosion()
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

void Enemy::Explode(double time)
{
	isExploding = true;
	for (int i = 0; i < 20; i++)
	{
		float vel = getRandom(-3.5f, 3.5f);
		float vel1 = getRandom(-3.5f, 3.5f);

		particles.push_back({ enemyShape.pos.x, enemyShape.pos.y, 0.005, vel, vel1, time });
	}
}