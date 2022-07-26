#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_audio.h>
#include "Helper.h"

class Enemy
{
public:
	struct ep
	{
		float x = 0.0f;
		float y = 0.0f;
		double updateFrequency = 0.005;
		float xVel = 0.0f;
		float yVel = 0.0f;
		double timer;
		float alpha = 255.0f;
	};
	std::vector<ep> particles;

public:
	int GUID = -1;
	float enemySize = 50.0f;
	float enemyHealth = 75.0f;
	float enemyMaxHealth = 75.0f;
	float enemySpeed = 0.75f;
	float distance = 0.0f;
	ALLEGRO_SAMPLE_ID* hum = nullptr;

	float damage = 0.0f;
	double attackSpeed = 0.5;
	double attackTimer = 0.0;
	bool readyToDelete = false;
	ALLEGRO_COLOR enemyColor = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR pointColor = al_map_rgb(255, 0, 0);
	std::vector<statusText*> dmgText;

	polygon enemyShape;

	void newEnemy(int playerLevel, float width, float height);
	void moveEnemy(float x, float y, bool colliding);
	void takeDamage(float damage);

	void Explode(double time);
	void checkExplosion();
	bool isExploding = false;
	bool isExploded = false;
};