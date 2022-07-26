#pragma once
#include <string>
#include <vector>
#include "Helper.h"
#include <allegro5/allegro_image.h>

class Player
{
public:
	Player(std::string name);
	~Player();

	std::string playerName = "";

	void* spaceship;

	float playerCenterX = 0.0f;
	float playerCenterY = 0.0f;

	float shootRange = 250.0f;

	float playerHealth = 100.0f;
	float playerMaxHealth = 100.0f;
	int playerLevel = 1;
	unsigned long long experience = 0;
	unsigned long long neededExperience[MAX_LEVEL];

	float accuracy = 0.65f;
	float playerDamage = 25.0f;
	double shootDelay = 0.60;
	float playerRegen = 1.0f;
	float healthMod = 0.0f;

	float accMod = 0.0f;
	float dmgMod = 0.0f;
	double shotDelayMod = 0.0;
	float regenMod = 0.0f;

	long long cash = 0;

	std::vector<statusText*> playerText;

	void LevelUp();
	void AddCash(long long addCash);
	void AddExperience(unsigned long long addExp);
	bool CheckLevelup();
	void takeDamage(float damage);
	void UpdateHealth(float mod);

	static unsigned long long getExperienceByLevel(int level)
	{
		return ((level * 100) * (level * 5) / MAX_LEVEL) * 1.5f;
	};

	float x = 0.0f;
	float y = 0.0f;

	polygon playerShape;
};

