#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>

#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Helper.h"
#include "Button.h"

enum
{
	LASER_SOUND = 0,
	IMPACT_SOUND = 1,
	ENEMY_HUM_SOUND = 2,
	ENEMY_EXPLOSION_SOUND = 3
};

class Tower
{
public:
	Tower();
	virtual ~Tower();

	std::vector<Projectile*> activeProjectiles;
	std::vector<Enemy*> activeEnemies;
	std::vector<Button*> activeButtons;
	Player* _player = nullptr;

	ALLEGRO_BITMAP* background;

	std::vector<ALLEGRO_SAMPLE*> soundEffect;
	ALLEGRO_DISPLAY* g_Window;

	double expBarTimer = 0.0;
	float barWidth = 0.0f;

	bool bInit(ALLEGRO_DISPLAY* display);

	void UpdateScreen();
	void DrawScreen();

	void DrawPlayer();
	void DrawEnemy(Enemy* e);
	void UpdateMouseDownState(unsigned int button, float x, float y);
	void UpdateMouseUpState(unsigned int button, float x, float y);
	void UpdateMouseAxis(float x, float y);
	void OnKeyUp(int keycode);
	bool LoadFont(std::string fileName, int size, int flags);
	void _DrawText(std::string text, int x, int y, ALLEGRO_FONT* font, ALLEGRO_COLOR rgb, int flags);
	ALLEGRO_FONT* getFont(int size, bool bold = false);

	bool checkTime(double* timer, double rate);
	bool checkMousePosition(float minx, float miny, float maxx, float maxy);
	template <typename T>
	bool checkDeletion(std::vector<T>* _array);

	void DrawCircle(int x0, int y0, int radius, ALLEGRO_COLOR color);
	void DrawButton(Button* b);
	void DrawButtonTips(Button* b);

	void PerkButton(Button* b, float* pMod, std::string perkName);
	void PerkButton(Button* b, double* pMod, std::string perkName);
	void HandleMouseOver();

	struct ec
	{
		int guid = -1;
		float distance = 0.0f;
	};
	Enemy* getClosestEnemy(float lastDistance, float pCX, float pCY);
	static bool sortByDistance(const ec& lhs, const ec& rhs);

	ALLEGRO_COLOR rgba(float r, float g, float b, float a)
	{
		return al_map_rgba((r * a) / 255, (g * a) / 255, (b * a) / 255, a);
	};

	float getAngle(int p1x, int p1y, int p2x, int p2y)
	{
		int deltaY = p2y - p1y;
		int deltaX = p2x - p1x;

		float angleInDegrees = atan2(deltaY, deltaX) * 180 / 3.141;

		angleInDegrees *= -1;
		return angleInDegrees;
	}
};

