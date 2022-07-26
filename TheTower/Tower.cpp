#include "Tower.h"

double shootTimer = 0.0;

int numEnemies = 5;
int deathCounter = 0;

float fpsCounter;
float lastFPS;
double fpsTimer;
double fpsDelay = 1.0;
double damageTimer = 0.0;
double regenTimer = 0.0;
double regenDelay = 1.0;

std::vector<vec2d> spawnPoints;

ALLEGRO_TRANSFORM* transform;

ALLEGRO_FONT* fonts[NUM_FONTS];
ALLEGRO_FONT* bFonts[NUM_FONTS];

Enemy* currentTarget = nullptr;
ALLEGRO_BITMAP* enemyShip = nullptr;

Tower::Tower()
{
	background = nullptr;
	al_reserve_samples(2000);
	g_Window = nullptr;
}

Tower::~Tower()
{
	for (int i = 0; i < NUM_FONTS; i++) al_destroy_font(fonts[i]);
	for (int i = 0; i < NUM_FONTS; i++) al_destroy_font(bFonts[i]);
	for(auto& sound : soundEffect) al_destroy_sample(sound);
}

bool Tower::bInit(ALLEGRO_DISPLAY* display)
{
	g_Window = display;
	w_Width = al_get_display_width(display);
	w_Height = al_get_display_height(display) - 200;

	// Init player
	_player = new Player("Player 1");

	// Init fonts
	for (int i = 0; i < NUM_FONTS; i++) fonts[i] = nullptr;
	for (int i = 0; i < NUM_FONTS; i++) bFonts[i] = nullptr;

	// Load fonts
	LoadFont(".\\comic.ttf", 14, 0);
	LoadFont(".\\comic.ttf", 12, 0);
	LoadFont(".\\comic.ttf", 10, 0);

	// Init shoot timer
	shootTimer = al_get_time();

	background = al_load_bitmap(".\\spaceback.png");
	_player->spaceship = al_load_bitmap(".\\spaceship.png");
	enemyShip = al_load_bitmap(".\\saucer.png");
	soundEffect.push_back(al_load_sample(".\\laser2.wav"));
	soundEffect.push_back(al_load_sample(".\\impact.mp3"));
	soundEffect.push_back(al_load_sample(".\\hum.wav"));
	soundEffect.push_back(al_load_sample(".\\explosion.wav"));

	// Get spawn points
	std::vector<vec2d> _tmp;
	int stepCounter = 0;
	for (int x = -200; x < (w_Width + 200); x++)
	{
		for (int y = -200; y < (w_Height + 200); y++)
		{
			stepCounter += 25;
			if (stepCounter >= 2000)
			{
				stepCounter = 0;
				vec2d d;
				d.x = x;
				d.y = y;
				_tmp.push_back(d);
			}
		}
	}
	for (auto& p : _tmp)
	{
		if (p.x <= -25 || p.x >= w_Width + 25 && p.y <= -25 || p.y >= w_Height + 25)
		{
			spawnPoints.push_back(p);
		}
	}

	// Spawn the initial enemies
	for (int i = 0; i < numEnemies; i++)
	{
		Enemy* _e = new Enemy;
		_e->GUID = getRandom(0, 1000);
		for (auto& e : activeEnemies)
		{
			while (_e->GUID == e->GUID)
			{
				_e->GUID = getRandom(0, 1000);
			}
		}
		vec2d spawn = spawnPoints[getRandom(0, spawnPoints.size() - 1)];
		_e->newEnemy(_player->playerLevel + 1, spawn.x, spawn.y);
		al_play_sample(soundEffect[ENEMY_HUM_SOUND], 0.01f, 0.0f, 0.5f, ALLEGRO_PLAYMODE_LOOP, _e->hum);
		activeEnemies.push_back(_e);
	}

	// Init buttons
	Button* blasterEff = new Button(new std::string[2]{ "Increase blaster", "efficiency" }, 2, ((15 + 125) * 0) + 15, 75 + w_Height, 125, 35, 12.0f);
	{
		blasterEff->buttonName = "blasterEff";
		blasterEff->cost = 65;
		blasterEff->mod = 0.015f;
		std::stringstream ss, ss1;
		ss << "by " << std::fixed << std::setprecision(1) << (blasterEff->mod * 100) << "%";
		ss1 << "Cost: $";
		blasterEff->AddTooltip(new std::string[3]{ "Increases blaster rate of fire", ss.str(), ss1.str() }, 3);
	}
	activeButtons.push_back(blasterEff);

	Button* blasterDmg = new Button("Increase Damage", ((15 + 125) * 1) + 15, 75 + w_Height, 125, 35);
	{
		blasterDmg->buttonName = "blasterDmg";
		blasterDmg->cost = 65;
		blasterDmg->mod = 0.15f;
		std::stringstream ss, ss1;
		ss << "by " << std::fixed << std::setprecision(1) << (blasterDmg->mod * 100) << "%";
		ss1 << "Cost: $";
		blasterDmg->AddTooltip(new std::string[3]{ "Increases blaster damage", ss.str(), ss1.str() }, 3);
	}
	activeButtons.push_back(blasterDmg);

	Button* playerRegen = new Button("Increase Regen", ((15 + 125) * 2) + 15, 75 + w_Height, 125, 35);
	{
		playerRegen->buttonName = "playerRegen";
		playerRegen->cost = 65;
		playerRegen->mod = 0.5f;
		std::stringstream ss, ss1;
		ss << "by " << std::fixed << std::setprecision(1) << (playerRegen->mod * 100) << "%";
		ss1 << "Cost: $";
		playerRegen->AddTooltip(new std::string[3]{ "Increases shield regeneration", ss.str(), ss1.str() }, 3);
	}
	activeButtons.push_back(playerRegen);

	Button* playerHealth = new Button(new std::string[2]{ "Increase Max", "Health" }, 2, ((15 + 125) * 3) + 15, 75 + w_Height, 125, 35, 12.0f);
	{
		playerHealth->buttonName = "playerHealth";
		playerHealth->cost = 65;
		playerHealth->mod = 5.0f;
		std::stringstream ss, ss1;
		ss << "by " << playerHealth->mod;
		ss1 << "Cost: $";
		playerHealth->AddTooltip(new std::string[3]{ "Increases available shields", ss.str(), ss1.str() }, 3);
	}
	activeButtons.push_back(playerHealth);

	return true;
}

void Tower::UpdateScreen()
{
	for (size_t i = 0; i < _player->playerShape.o.size(); i++)
	{
		_player->playerShape.p[i] = {
			(_player->playerShape.o[i].x * cosf(_player->playerShape.angle)) - (_player->playerShape.o[i].y * sinf(_player->playerShape.angle)) + _player->playerShape.pos.x,
			(_player->playerShape.o[i].x * sinf(_player->playerShape.angle)) + (_player->playerShape.o[i].y * cosf(_player->playerShape.angle)) + _player->playerShape.pos.y,
		};
		_player->playerShape.overlap = false;
	}

	for (auto& e : activeEnemies)
	{
		for (size_t i = 0; i < e->enemyShape.o.size(); i++)
		{
			e->enemyShape.p[i] = {
				(e->enemyShape.o[i].x * cosf(e->enemyShape.angle)) - (e->enemyShape.o[i].y * sinf(e->enemyShape.angle)) + e->enemyShape.pos.x,
				(e->enemyShape.o[i].x * sinf(e->enemyShape.angle)) + (e->enemyShape.o[i].y * cosf(e->enemyShape.angle)) + e->enemyShape.pos.y
			};
			e->enemyShape.overlap = false;
		}

		if (e->isExploded == true && e->readyToDelete)
		{
			if(currentTarget != nullptr) if (currentTarget->GUID == e->GUID) currentTarget = nullptr;
			// increase kill counter
			deathCounter++;
			// Add player money
			_player->AddCash(e->enemyMaxHealth * 0.5 + 0.5);
			// Add experience points
			_player->AddExperience((e->enemyMaxHealth / 100.0) + 1.0);
			// Handle player levelup
			if (_player->CheckLevelup())
			{
				_player->LevelUp();
				barWidth = 0.0f;
			}
		}
	}

	// Add new enemies
	if (numEnemies < 5 + (int)(_player->playerLevel / 2))
	{
		Enemy* _e = new Enemy;
		_e->GUID = getRandom(0, 1000);
		for (auto& e : activeEnemies)
		{
			while (_e->GUID == e->GUID)
			{
				_e->GUID = getRandom(0, 1000);
			}
		}
		vec2d spawn = spawnPoints[getRandom(0, spawnPoints.size() - 1)];
		_e->newEnemy(_player->playerLevel + 1, spawn.x, spawn.y);
		al_play_sample(soundEffect[ENEMY_HUM_SOUND], 0.01f, 0.0f, 0.5f, ALLEGRO_PLAYMODE_LOOP, _e->hum);
		activeEnemies.push_back(_e);
		numEnemies++;
	}

	// Move enemy and check distance between enemy and player
	for (auto& e : activeEnemies)
	{
		bool isCollision = ShapeOverlap_SAT(e->enemyShape, _player->playerShape);
		e->moveEnemy(_player->playerShape.pos.x, _player->playerShape.pos.y, isCollision);
		if (isCollision)
		{
			if (checkTime(&e->attackTimer, e->attackSpeed)) _player->takeDamage(e->damage);
		}

		e->checkExplosion();
	}

	if (checkTime(&regenTimer, regenDelay))
	{
		if (_player->playerHealth < _player->playerMaxHealth)
		{
			std::stringstream ss;
			ss << "+" << std::fixed << std::setprecision(2) << (_player->playerRegen + _player->regenMod);
			statusText* _ss = new statusText;
			_ss->str = new std::string[1]{
				ss.str()
			};
			_ss->x = (195.0f / 2.0f) + 10.0f;
			_ss->y = w_Height + 5;
			_ss->strCount = 1;
			_player->playerText.push_back(_ss);
			_player->playerHealth += (_player->playerRegen + _player->regenMod);
			if (_player->playerHealth >= _player->playerMaxHealth) _player->playerHealth = _player->playerMaxHealth;
		}
	}

	// Init projectile PERK MAYBE?
	//if (currentTarget == nullptr) currentTarget = getClosestEnemy(_player->shootRange, _player->playerCenterX, _player->playerCenterY);
	//if (al_get_time() - shootTimer >= _player->shootDelay)
	//{
	//	shootTimer = al_get_time();

	//	// Calculate the nearest enemy to shoot
	//	if (currentTarget != nullptr)
	//	{
	//		al_play_sample(soundEffect[LASER_SOUND], 0.25f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, 0);
	//		currentTarget->enemyColor = rgba(255, 0, 0, 255);
	//		Projectile* p = new Projectile();
	//		float accuracy = (-(_player->accuracy) * 100) + 100;
	//		if (getDistance(_player->playerCenterX, _player->playerCenterY, currentTarget->enemyShape.pos.x, currentTarget->enemyShape.pos.x) <= 50.0f + (currentTarget->enemySize * 0.8f))
	//		{
	//			accuracy = (-(0.7f) * 100.0f) + 100.0f;
	//		}
	//		p->InitProjectile(_player->playerCenterX, _player->playerCenterY, al_get_time(),
	//			currentTarget->enemyShape.pos.x + getRandom(-accuracy, accuracy),
	//			currentTarget->enemyShape.pos.y + getRandom(-accuracy, accuracy));
	//		activeProjectiles.push_back(p);
	//	}
	//}

	if (mouseState.isLeftButtonDown && checkMousePosition(0,0,w_Width, w_Height)/*mouseState.x >= 0 && mouseState.y >= 0 && mouseState.x < w_Width && mouseState.y < w_Height*/)
	{
		if (checkTime(&shootTimer, (_player->shootDelay - (_player->shootDelay * _player->shotDelayMod))))
		{
			al_play_sample(soundEffect[LASER_SOUND], 0.25f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, 0);
			Projectile* p = new Projectile();
			float accuracy = (-(_player->accuracy) * 100) + 100;
			p->InitProjectile(_player->playerCenterX, _player->playerCenterY, al_get_time(),
				mouseState.x + getRandom(-accuracy, accuracy),
				mouseState.y + getRandom(-accuracy, accuracy));
			activeProjectiles.push_back(p);
		}
	}

	// Fire and move projectile in proper direction
	for (auto& p : activeProjectiles)
	{
		// Check if projectile is off screen
		p->CheckForDeletion();
		// Move Projectile
		p->moveProjectile();

		// Check for collision of the projectile and enemy
		for (auto& e : activeEnemies)
		{
			if (!e->isExploded && !e->isExploding)
			{
				if (p->CheckCollision(e->enemyShape.pos.x, e->enemyShape.pos.y, e->enemySize) && p->isExploding == false && p->isExploded == false)
				{
					e->takeDamage((_player->playerDamage * (_player->dmgMod + 1.0f)));
					p->Explode(al_get_time());
					if (e->enemyHealth <= 0)
					{
						al_play_sample(soundEffect[ENEMY_EXPLOSION_SOUND], 0.15f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, 0);
					}
					else {
						al_play_sample(soundEffect[IMPACT_SOUND], 0.25f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, 0);
					}
				}
			}
		}

		p->checkExplosion();

		float vel = getRandom(-1.5f, 1.5f);
		float vel1 = getRandom(-1.5f, 1.5f);
		p->shootParticles.push_back(Projectile::pp { p->projX, p->projY, 0.005, vel, vel1, al_get_time() });

		p->checkLaserParticles();
	}

	// Delete any projectiles as needed
	checkDeletion(&activeProjectiles);
	// Check if enemies are ready to be deleted
	if (checkDeletion(&activeEnemies)) numEnemies--;

	for (auto& b : activeButtons)
	{
		PerkButton(b, &_player->shotDelayMod, "blasterEff");
		PerkButton(b, &_player->dmgMod, "blasterDmg");
		PerkButton(b, &_player->regenMod, "playerRegen");
		PerkButton(b, &_player->healthMod, "playerHealth");
	}
}

void Tower::DrawScreen()
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(background, 0, 0, 0);

	// Draw shot range
	//int alpha = 100;
	//al_draw_circle(_player->playerCenterX, _player->playerCenterY, _player->shootRange, rgba(255, 255, 255, 25), 1.0f);
	//al_draw_circle(_player->playerCenterX, _player->playerCenterY, 50.0f, rgba(255, 0, 0, 15), 1.0f);
	
	// Draw player
	DrawPlayer();

	// Draw active enenmies
	for (auto& e : activeEnemies)
	{
		if (e->isExploding)
		{
			for (auto& _p : e->particles)
			{
				al_draw_filled_rectangle(_p.x - 2, _p.y - 2, _p.x + 2, _p.y + 2,
					rgba(255, 50, 50, e->particles[0].alpha));
			}
		}

		if(!e->isExploded && !e->isExploding)
		{
			DrawEnemy(e);

			if(checkMousePosition(e->enemyShape.pos.x - (e->enemySize / 2), e->enemyShape.pos.y - (e->enemySize / 2),
				e->enemyShape.pos.x + (e->enemySize / 2), e->enemyShape.pos.y + (e->enemySize / 2)))
			{
				al_draw_rectangle(e->enemyShape.pos.x - (e->enemySize / 2), e->enemyShape.pos.y - (e->enemySize / 2) - 10.0f,
					e->enemyShape.pos.x + (e->enemySize / 2), e->enemyShape.pos.y - (e->enemySize / 2) - 5.0f, al_map_rgb(255, 255, 255), 1.0f);

				al_draw_filled_rectangle(e->enemyShape.pos.x - (e->enemySize / 2), e->enemyShape.pos.y - (e->enemySize / 2) - 10.0f,
					e->enemyShape.pos.x + (e->enemySize / 2) - e->enemySize + ((e->enemySize / e->enemyMaxHealth) * e->enemyHealth),
					e->enemyShape.pos.y - (e->enemySize / 2) - 5.0f, al_map_rgb(255, 255, 255));

				std::stringstream health;
				health << "HP: " << (int)e->enemyHealth << "/" << (int)e->enemyMaxHealth;
				_DrawText(health.str(), e->enemyShape.pos.x - (al_get_text_width(getFont(10), health.str().c_str()) / 2), e->enemyShape.pos.y - (e->enemySize / 2) - 25.0f, getFont(10), al_map_rgb(255, 255, 255), 0);
			}
		}
		//std::stringstream dist;
		//dist << (int)e->distance;
		//_DrawText(dist.str(), e->enemyShape.pos.x - (al_get_text_width(getFont(10), dist.str().c_str()) / 2), e->enemyShape.pos.y + (e->enemySize / 2) + 5.0f, getFont(10), al_map_rgb(255, 255, 255), 0);
	}

	// Draw all active projectiles
	for (auto& p : activeProjectiles)
	{
		if (p->isExploding)
		{
			for (auto& _p : p->particles)
			{
				al_draw_pixel(_p.x, _p.y, rgba(255, 255, 255, _p.alpha));
			}
		}
		else {
			for (auto& _p : p->shootParticles)
			{
				al_draw_pixel(_p.x, _p.y, rgba(155, 0, 0, _p.alpha));
			}
			if (!p->isExploded)
			{
				if (p->lastPositions.size() > 0)
				{
					Projectile::lp laser = p->getLaser((int)p->projectileSpeed + 2.0f);
					al_draw_line(laser.begin.x, laser.begin.y, laser.end.x, laser.end.y, rgba(255, 0, 0, 255), 2.0f);
					al_draw_filled_circle(laser.end.x, laser.end.y, 2.0f, rgba(200, 0, 0, 255));
				}
			}
		}
	}

	fpsCounter++;
	std::stringstream fps;
	fps << "FPS: " << lastFPS;
	if(checkTime(&fpsTimer, fpsDelay))
	{
		lastFPS = fpsCounter;
		fpsCounter = 0;
	}
	_DrawText(fps.str(), 5, 0, getFont(14), al_map_rgb(255, 255, 255), 0);

	al_draw_filled_rectangle(0.0f, w_Height, w_Width, w_Height + 200.0f, rgba(0, 0, 0, 255));

	// Player Status
	al_draw_rectangle(5, w_Height + 5, (w_Width / 2) - 5, w_Height + 65, rgba(255, 255, 255, 100), 1.0f);
	{
		int numBars = 10;
		al_draw_filled_rectangle(0.0f, w_Height - 7.0f, w_Width, w_Height, rgba(0, 0, 0, 255));
		float max = (w_Width / _player->neededExperience[_player->playerLevel + 1]) * _player->experience;
		if (barWidth < max)
		{
			if (al_get_time() - expBarTimer >= 0.0005)
			{
				barWidth++;
				expBarTimer = al_get_time();
			}
		}
		al_draw_filled_rectangle(0.0f, w_Height - 6.5f, barWidth, w_Height - 1.0f, rgba(50, 200, 50, 255));
		for (int i = (w_Width / numBars); i <= (int)w_Width; i += w_Width / numBars)
		{
			al_draw_rectangle(i - (w_Width / numBars) + 1, w_Height - 7.0f, i, w_Height, rgba(100, 100, 100, 255), 1.0f);
		}

		float px = 5;
		float py = w_Height + 5;
		float pw = (w_Width / 2) - 5;
		float ph = w_Height + 65;

		std::string nameLabel = "Player: ";
		_DrawText(nameLabel, px + 5, py, getFont(12), al_map_rgb(255, 255, 255), 0);
		_DrawText(_player->playerName, px + 5 + al_get_text_width(getFont(12), nameLabel.c_str()), py, getFont(12), al_map_rgb(100, 255, 100), 0);

		// Hacky bullshit
		std::stringstream ss;
		ss << (int)_player->playerHealth << "/" << (int)_player->playerMaxHealth;
		_DrawText(ss.str(), px + 5 + ((195.0f - al_get_text_width(getFont(12), ss.str().c_str())) / 2), w_Height + 23, getFont(12), al_map_rgb(255, 255, 255), 0);
		al_draw_rectangle(px + 5, py + 20, px + 200, py + 20 + 15, al_map_rgb(255, 255, 255), 1.0f);
		ALLEGRO_BITMAP* bmp = al_create_bitmap(200, 20);
		{
			al_set_target_bitmap(bmp);
			al_draw_filled_rectangle(5, 0, 5 + ((195.0f / _player->playerMaxHealth) * _player->playerHealth), 15, al_map_rgb(255, 255, 255));
			std::stringstream ss;
			ss << (int)_player->playerHealth << "/" << (int)_player->playerMaxHealth;
			_DrawText(ss.str(), 5 + ((195.0f - al_get_text_width(getFont(12), ss.str().c_str())) / 2), -2, getFont(12), al_map_rgb(155, 0, 0), 0);
			al_set_target_backbuffer(g_Window);
			al_draw_bitmap_region(bmp, 0, 0, 5 + ((195.0f / _player->playerMaxHealth) * _player->playerHealth), 20, px, py + 20, 0);
		}
		al_destroy_bitmap(bmp);

		std::stringstream level;
		level << "Level: " << _player->playerLevel;
		_DrawText(level.str(), px + 5, py + 40, getFont(12), al_map_rgb(255, 255, 255), 0);

		std::stringstream money;
		money << _player->cash;
		int offset = al_get_text_width(getFont(12), money.str().c_str()) + 10;
		_DrawText("$", (px + pw) - offset - 10, py, getFont(14), al_map_rgb(100, 255, 100), 0);
		_DrawText(money.str(), (px + pw) - offset, py, getFont(12), al_map_rgb(100, 255, 100), 0);
	}

	// Player Stats
	al_draw_rectangle((w_Width / 2), w_Height + 5, w_Width - 5, w_Height + 65, rgba(255, 255, 255, 100), 1.0f);
	{

	}

	for (auto& b : activeButtons) DrawButton(b);
	for (auto& b : activeButtons) DrawButtonTips(b);

	for (auto& e : activeEnemies)
	{
		// Show the damage delt in a moving and fading text above the enemy
		for (auto& dt : e->dmgText)
		{
			std::stringstream ss;
			for (int i = 0; i < dt->strCount; i++) ss << dt->str[i];
			_DrawText(ss.str(), e->enemyShape.pos.x - (al_get_text_width(getFont(10), ss.str().c_str()) / 2),
				e->enemyShape.pos.y - (e->enemySize / 2) - 40.0f - dt->textMoveOffsetY, getFont(10), rgba(200, 0, 0, dt->textFadeAlpha), 0);

			if (checkTime(&dt->textMoveTimer, dt->textMoveDelay)) dt->textMoveOffsetY += 1.0f;

			if (checkTime(&dt->textFadeTimer, dt->textFadeDelay))
			{
				dt->textFadeAlpha -= 15.0f;
				if (dt->textFadeAlpha <= 0.0f)
				{
					dt->readyToDelete = true;
				}
			}
		}
		if (e->dmgText.size() > 0)
		{
			for (auto it = e->dmgText.begin(); it != e->dmgText.end();)
			{
				if ((*it)->readyToDelete)
				{
					delete* it;
					it = e->dmgText.erase(it);
				}
				else {
					it++;
				}
			}
		}
	}

	for (auto& pt : _player->playerText)
	{
		std::stringstream ss;
		for (int i = 0; i < pt->strCount; i++) ss << pt->str[i];
		_DrawText(ss.str(), pt->x - (al_get_text_width(getFont(10), ss.str().c_str()) / 2), pt->y - pt->textMoveOffsetY,
			getFont(10), rgba(0, 255, 0, pt->textFadeAlpha), 0);

		if (checkTime(&pt->textMoveTimer, pt->textMoveDelay)) pt->textMoveOffsetY += 1.0f;

		if (checkTime(&pt->textFadeTimer, pt->textFadeDelay))
		{
			pt->textFadeAlpha -= 15.0f;
			if (pt->textFadeAlpha <= 0.0f)
			{
				pt->readyToDelete = true;
			}
		}
	}

	if (_player->playerText.size() > 0)
	{
		for (auto it = _player->playerText.begin(); it != _player->playerText.end();)
		{
			if ((*it)->readyToDelete)
			{
				delete* it;
				it = _player->playerText.erase(it);
			}
			else {
				it++;
			}
		}
	}

	HandleMouseOver();

	al_flip_display();
}

void Tower::HandleMouseOver()
{
	if (checkMousePosition(0.0f, w_Height - 7.0f, w_Width, w_Height))
	{
		std::stringstream ss;
		ss << _player->experience << "/" << _player->neededExperience[_player->playerLevel + 1];
		float textWidth = al_get_text_width(getFont(12), ss.str().c_str());
		al_draw_filled_rectangle(mouseState.x, mouseState.y - 15, mouseState.x + 5 + (textWidth)+5, mouseState.y + 5, rgba(0, 0, 0, 100));
		al_draw_rectangle(mouseState.x, mouseState.y - 15, mouseState.x + 5 + textWidth + 5, mouseState.y + 5, al_map_rgb(255, 255, 255), 1.0f);
		_DrawText(ss.str(), mouseState.x + 5, mouseState.y - 15, getFont(12), al_map_rgb(255, 255, 255), 0);
	}
}

void Tower::DrawButtonTips(Button* b)
{
	ALLEGRO_COLOR tcolor = b->textColor;

	if (b->checkButtonHover())
	{
		tcolor = b->textColor_Hover;

		float lineHeight = 0.0f;
		float lineWidth = 0.0f;
		for (int i = 0; i < b->tipCount; i++)
		{
			lineHeight += 12.0f;
			if (lineWidth <= al_get_text_width(getFont(12), b->toolTip[i].c_str())) lineWidth = al_get_text_width(getFont(12), b->toolTip[i].c_str());
		}

		al_draw_filled_rectangle(mouseState.x + 10, mouseState.y - (lineHeight), mouseState.x + 5 + lineWidth + 15, mouseState.y + 5, rgba(0, 0, 0, 200));
		al_draw_rectangle(mouseState.x + 10, mouseState.y - (lineHeight), mouseState.x + 5 + lineWidth + 15, mouseState.y + 5, al_map_rgb(255, 255, 255), 1.0f);

		int tip = 0;
		for (int i = b->tipCount - 1; i >= 0; i--)
		{
			std::string str = b->toolTip[tip];
			bool bold = false;
			if (b->toolTip[tip].substr(0, 7) == "Cost: $")
			{
 				str.append(std::to_string(b->hoveringCost));
				bold = true;
				tcolor = al_map_rgb(155, 255, 155);
			}
			_DrawText(str, mouseState.x + 15, mouseState.y + (lineHeight - (12.0f * i)) - 50.0f, getFont(12, bold), tcolor, 0);
			tip++;
		}
	}
}

void Tower::DrawButton(Button* b)
{
	ALLEGRO_COLOR bcolor = b->buttonColor;
	ALLEGRO_COLOR tcolor = b->textColor;
	if (b->checkButtonHover())
	{
		bcolor = b->buttonColor_Hover;
		tcolor = b->textColor_Hover;
	}
	if (b->isDisabled)
	{
		bcolor = b->buttonDisabled;
		tcolor = b->buttonDisabled;
	}
	al_draw_rounded_rectangle(b->x, b->y, b->x + b->width, b->y + b->height, 1.0f, 1.0f, bcolor, 1.0f);

	if (b->isMultiline)
	{
		for (int i = 0; i < b->lineCount; i++)
			_DrawText(b->multiText[i], b->x + ((b->width - al_get_text_width(getFont(12), b->multiText[i].c_str())) / 2), b->y + (b->lineHeight * i), getFont(12), tcolor, 0);
	}
	else {
		_DrawText(b->text, b->x + ((b->width - al_get_text_width(getFont(12), b->text.c_str())) / 2), b->y + ((b->height - al_get_font_line_height(getFont(12))) / 2) - 2, getFont(12), tcolor, 0);
	}
}

void Tower::DrawEnemy(Enemy* e)
{
	//al_draw_filled_rectangle(e->enemyShape.pos.x - (e->enemySize / 2.0f), e->enemyShape.pos.y - (e->enemySize / 2.0f), e->enemyShape.pos.x + (e->enemySize / 2), e->enemyShape.pos.y + (e->enemySize / 2), rgba(0, 0, 0, 255));
	//for (size_t i = 0; i < e->enemyShape.p.size(); i++)
	//{
	//	float x = e->enemyShape.p[i].x;
	//	float y = e->enemyShape.p[i].y;
	//	float x1 = e->enemyShape.p[(i + 1) % e->enemyShape.p.size()].x;
	//	float y1 = e->enemyShape.p[(i + 1) % e->enemyShape.p.size()].y;
	//	al_draw_line(x, y, x1, y1, e->enemyColor, 1.0f);
	//}
	e->enemyShape.angle += 0.0025;
	al_draw_scaled_rotated_bitmap(enemyShip, (155 / 2), (149 / 2), e->enemyShape.pos.x, e->enemyShape.pos.y,
		e->enemySize / 100, e->enemySize / 100, e->enemyShape.angle, 0);
}

void Tower::DrawPlayer()
{
	//DrawCircle(_player->playerShape.pos.x, _player->playerShape.pos.y, 50, rgba(0, 0, 0, 255.0f));
	//for (size_t i = 0; i < _player->playerShape.p.size(); i++)
	//{
	//	float x = _player->playerShape.p[i].x;
	//	float y = _player->playerShape.p[i].y;
	//	float x1 = _player->playerShape.p[(i + 1) % _player->playerShape.p.size()].x;
	//	float y1 = _player->playerShape.p[(i + 1) % _player->playerShape.p.size()].y;
	//	al_draw_line(x, y, x1, y1, al_map_rgb(255, 255, 255), 1.0f);
	//}
	float angle = atan2(_player->playerCenterY - mouseState.y, _player->playerCenterX - mouseState.x);
	al_draw_rotated_bitmap((ALLEGRO_BITMAP*)_player->spaceship, (81 / 2), (84 / 2), _player->playerCenterX, _player->playerCenterY, angle - 1.55f, 0);
}

void Tower::DrawCircle(int x0, int y0, int radius, ALLEGRO_COLOR color)
{
	unsigned char r, g, b, a;
	al_unmap_rgba(color, &r, &g, &b, &a);
	unsigned char app = 0.0;
	for (int i = radius; i > 0; i--)
	{
		if (i < (radius * 0.95f))
		{
			app += 0.5;
		}
		al_draw_circle(x0, y0, i, rgba(r, g, b, (a - ((a / radius) * i)) + app), 2.0f);
	}
}

void Tower::UpdateMouseAxis(float x, float y)
{
	mouseState.x = x;
	mouseState.y = y;
}

void Tower::UpdateMouseDownState(unsigned int button, float x, float y)
{
	mouseState.x = x;
	mouseState.y = y;
	if (button == 1)
	{
		mouseState.isLeftButtonDown = true;
	}
	if (button == 2)
	{
		mouseState.isRightButtonDown = true;
	}
	mouseState.didClick = true;
}

void Tower::UpdateMouseUpState(unsigned int button, float x, float y)
{
	mouseState.x = x;
	mouseState.y = y;
	if (button == 1)
	{
		mouseState.isLeftButtonDown = false;
	}
	if (button == 2)
	{
		mouseState.isRightButtonDown = false;
	}
	mouseState.didClick = false;
}

void Tower::OnKeyUp(int keycode)
{
	switch (keycode)
	{
	case ALLEGRO_KEY_SPACE:
		_player->AddCash(1000);
		break;
	}
}

bool Tower::LoadFont(std::string fileName, int size, int flags)
{
	if (g_Window == nullptr) return false;
	if (size <= NUM_FONTS - 1)
	{
		std::ifstream f(fileName.c_str());
		if (f.good())
		{
			fonts[size] = al_load_ttf_font(fileName.c_str(), size, flags);
		}
		std::string boldFileName = fileName.substr(0, fileName.length() - 4).append("_bold").append(fileName.substr(fileName.length() - 4, 4));
		{
			std::ifstream b(boldFileName.c_str());
			if (b.good())
			{
				bFonts[size] = al_load_ttf_font(boldFileName.c_str(), size, flags);
			}
		}
		if (!fonts[size]) return false;
		return true;
	}
	return false;
}

void Tower::_DrawText(std::string text, int x, int y, ALLEGRO_FONT* font, ALLEGRO_COLOR rgb, int flags)
{
	if (font == nullptr) return;
	int next = 0;
	al_hold_bitmap_drawing(true);
	for (size_t i = 0; i < text.length(); i++)
	{
		al_draw_glyph(font, rgb, x + next, y, text[i]);
		next += al_get_glyph_advance(font, text[i], flags);
	}
	al_hold_bitmap_drawing(false);
}

ALLEGRO_FONT* Tower::getFont(int size, bool bold)
{
	if (bold)
	{
		if (bFonts[size] != nullptr)
		{
			return bFonts[size];
		}
	}
	else {
		if (fonts[size] != nullptr)
		{
			return fonts[size];
		}
	}
	return nullptr;
}

Enemy* Tower::getClosestEnemy(float lastDistance, float pCX, float pCY)
{
	if (lastDistance < 1.0f) lastDistance = 1.0f;
	std::vector<ec> enemiesNear;

	for (auto& e : activeEnemies)
	{
		if (e->distance < lastDistance)
		{
			enemiesNear.push_back({ e->GUID, e->distance });
		}
	}
	if (enemiesNear.size() > 0)
	{
		std::sort(enemiesNear.begin(), enemiesNear.end(), sortByDistance);
		for (auto& e : activeEnemies)
		{
			if (e->GUID == enemiesNear[0].guid)
			{
				return e;
			}
		}
	}
	return nullptr;
}

bool Tower::sortByDistance(const ec& lhs, const ec& rhs)
{
	return lhs.distance < rhs.distance;
}

bool Tower::checkTime(double* timer, double rate)
{
	if (al_get_time() - *timer >= rate)
	{
		*timer = al_get_time();
		return true;
	}
	return false;
}

void Tower::PerkButton(Button* b, float* pMod, std::string perkName)
{
	if (b->buttonName != perkName) return;

	long long actualCost = b->cost * ((long long)(*pMod / b->mod) + 1);
	if (_player->cash < actualCost) b->isDisabled = true;
	else b->isDisabled = false;

	if (!b->checkButtonHover()) return;

	b->hoveringCost = actualCost;
	if (b->checkButtonClick())
	{
		if (_player->cash >= actualCost)
		{
			*pMod += b->mod;
			_player->AddCash(-(actualCost));
		}
	}
}

void Tower::PerkButton(Button* b, double* pMod, std::string perkName)
{
	if (b->buttonName == perkName)
	{
		long long actualCost = b->cost * ((long long)(*pMod / b->mod) + 1);
		if (_player->cash < actualCost) b->isDisabled = true;
		else b->isDisabled = false;

		if (b->checkButtonHover())
		{
			b->hoveringCost = actualCost;
			if (b->checkButtonClick())
			{
				if (_player->cash >= actualCost)
				{
					*pMod += b->mod;
					_player->AddCash(-(actualCost));
				}
			}
		}
	}
}

bool Tower::checkMousePosition(float minx, float miny, float maxx, float maxy)
{
	if (mouseState.x >= minx && mouseState.y >= miny && mouseState.x < maxx && mouseState.y < maxy) return true;
	return false;
}

template <typename T>
bool Tower::checkDeletion(std::vector<T>* _array)
{
	if ((*_array).size() > 0)
	{
		for (auto it = (*_array).begin(); it != (*_array).end();)
		{
			if ((*it)->isExploding == false)
			{
				if ((*it)->readyToDelete)
				{
					delete* it;
					it = (*_array).erase(it);
					return true;
				}
				else {
					it++;
				}
			}
			else {
				if ((*it)->particles[0].alpha <= 0.0f)
				{
					(*it)->isExploding = false;
					(*it)->isExploded = true;
				}
				it++;
			}
		}
	}
	return false;
}