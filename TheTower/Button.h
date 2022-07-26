#pragma once
#include "Helper.h"
#include <allegro5/allegro_color.h>

class Button
{
public:
	long long hoveringCost;

public:
	Button(std::string text, float x, float y, float width, float height);
	Button(std::string* text, int lineCount, float x, float y, float width, float height, float lineHeight);
	~Button();

	std::string buttonName;
	std::string text;
	std::string* multiText;
	std::string* toolTip;
	int tipCount = 0;

	bool clicked = false;
	bool isDisabled = false;

	long long cost = 50.0f;
	float mod = 0.0f;

	bool isMultiline = false;
	int lineCount = 0;
	float lineHeight = 0.0f;

	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;

	ALLEGRO_COLOR buttonDisabled;
	ALLEGRO_COLOR buttonColor;
	ALLEGRO_COLOR buttonColor_Hover;
	ALLEGRO_COLOR textColor;
	ALLEGRO_COLOR textColor_Hover;

	void AddTooltip(std::string* tipMsg, int lineCount);
	bool checkButtonClick();
	bool checkButtonHover();
};

