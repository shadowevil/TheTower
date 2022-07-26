#include "Helper.h"

MouseState mouseState;
float w_Width;
float w_Height;

float getRandom(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

int getRandom(int min, int max)
{
	return min + rand() % (max - min);
}

void getDirection(float px, float py, float ex, float ey, float* dirX, float* dirY)
{
	float dx = px - ex;
	float dy = py - ey;
	float distance = sqrt((dx * dx) + (dy * dy));
	*dirX = dx / distance;
	*dirY = dy / distance;
}

float getDistance(float px, float py, float ex, float ey)
{
	float dx = px - ex;
	float dy = py - ey;
	return sqrt((dx * dx) + (dy * dy));
};

bool ShapeOverlap_SAT(polygon& r1, polygon& r2)
{
	polygon* poly1 = &r1;
	polygon* poly2 = &r2;

	for (int shape = 0; shape < 2; shape++)
	{
		if (shape == 1)
		{
			poly1 = &r2;
			poly2 = &r1;
		}

		for (size_t a = 0; a < poly1->p.size(); a++)
		{
			int b = (a + 1) % poly1->p.size();
			vec2d axisProj = { -(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x };
			float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
			axisProj = { axisProj.x / d, axisProj.y / d };

			float min_r1 = INFINITY, max_r1 = -INFINITY;
			for (size_t p = 0; p < poly1->p.size(); p++)
			{
				float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
				min_r1 = std::min(min_r1, q);
				max_r1 = std::max(max_r1, q);
			}

			float min_r2 = INFINITY, max_r2 = -INFINITY;
			for (size_t p = 0; p < poly2->p.size(); p++)
			{
				float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
				min_r2 = std::min(min_r2, q);
				max_r2 = std::max(max_r2, q);
			}

			if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
			{
				return false;
			}
		}
		return true;
	}
}