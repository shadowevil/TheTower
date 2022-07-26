#include "Player.h"
#include <allegro5/allegro5.h>

Player::Player(std::string name)
{
	playerName = name;
	for (int i = 0; i < MAX_LEVEL; i++) neededExperience[i] = getExperienceByLevel(i);

	playerHealth = playerMaxHealth;
	playerCenterX = (w_Width - 1) / 2;
	playerCenterY = (w_Height + 5) / 2;
	x = (w_Width - 15.0f) / 2.0f;
	y = (w_Height - 40.0f) / 2.0f;
	float fTheta = 3.14159f * 2.0f / 5.0f;
	playerShape.pos = { playerCenterX, playerCenterY };
	playerShape.angle = 0.93f;
	for (int i = 0; i < 5; i++)
	{
		playerShape.o.push_back({ 30.0f * cosf(fTheta * i), 30.0f * sinf(fTheta * i) });
		playerShape.p.push_back({ 30.0f * cosf(fTheta * i), 30.0f * sinf(fTheta * i) });
	}
}

Player::~Player()
{

}

bool Player::CheckLevelup()
{
	return experience >= neededExperience[playerLevel + 1];
}

void Player::LevelUp()
{
	statusText* plyr = new statusText;
	plyr->str = new std::string[1]{
		"Levelup!"
	};
	plyr->strCount = 1;
	plyr->x = playerCenterX;
	plyr->y = playerCenterY;
	playerText.push_back(plyr);
	playerLevel++;
	AddCash(50);
	experience = 0;
}

void Player::AddCash(long long addCash)
{
	cash += addCash;
}

void Player::AddExperience(unsigned long long addExp)
{
	this->experience += addExp;
}

void Player::takeDamage(float damage)
{
	playerHealth -= damage;
	if (playerHealth < 0) playerHealth = 0;
}

void Player::UpdateHealth(float mod)
{
	playerMaxHealth += mod;
}