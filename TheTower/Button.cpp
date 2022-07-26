#include "Button.h"

Button::Button(std::string text, float x, float y, float width, float height)
{
	this->text = text;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	buttonColor = al_map_rgb(255, 255, 255);
	buttonColor_Hover = al_map_rgb(255, 0, 0);
	textColor = al_map_rgb(255, 255, 255);
	textColor_Hover = al_map_rgb(255, 255, 255);
	buttonDisabled = al_map_rgb(100, 100, 100);
}

Button::Button(std::string* text, int lineCount, float x, float y, float width, float height, float lineHeight)
{
	isMultiline = true;
	this->multiText = text;
	this->lineCount = lineCount;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->lineHeight = lineHeight;
	buttonColor = al_map_rgb(255, 255, 255);
	buttonColor_Hover = al_map_rgb(255, 0, 0);
	textColor = al_map_rgb(255, 255, 255);
	textColor_Hover = al_map_rgb(255, 255, 255);
	buttonDisabled = al_map_rgb(100, 100, 100);
}

void Button::AddTooltip(std::string* tipMsg, int l_tipCount)
{
	this->toolTip = tipMsg;
	this->tipCount = l_tipCount;
}

Button::~Button()
{

}

bool Button::checkButtonClick()
{
	if (mouseState.x >= x && mouseState.x <= x + width &&
		mouseState.y >= y && mouseState.y <= y + height)
	{
		if (mouseState.didClick == true)
		{
			clicked = true;
		}
	}

	if (clicked && mouseState.x >= x && mouseState.x <= x + width &&
		mouseState.y >= y && mouseState.y <= y + height)
	{
		if (mouseState.didClick == false)
		{
			clicked = false;
			return true;
		}
	}
	else {
		if (mouseState.didClick == false)
		{
			clicked = false;
		}
	}
	return false;
}

bool Button::checkButtonHover()
{
	if (mouseState.x >= this->x && mouseState.y >= this->y && mouseState.x < this->x + this->width && mouseState.y < this->y + this->height) return true;
	return false;
}