#include "./Vector2.h"

void driveVec(Vector2 vec, float* motor)
{
	float motorMultiplier[4] = {0, 0, 0, 0};

	motorMultiplier[0] = -vec.y;
	motorMultiplier[1] = -vec.y;
	motorMultiplier[2] = vec.y;
	motorMultiplier[3] = vec.y;

	motorMultiplier[0] += vec.x;
	motorMultiplier[1] += -vec.x;
	motorMultiplier[2] += -vec.x;
	motorMultiplier[3] += vec.x;

	int biggest_multiplier = 0;
	for (int i = 0; i < 4; i++)
	{
		if (motorMultiplier[biggest_multiplier] < motorMultiplier[i])
		{
			biggest_multiplier = i;
		}
	}

	int mult = motorMultiplier[biggest_multiplier];
	for (int i = 0; i < 4; i++)
	{
		motorMultiplier[i] /= mult;
	}

	int speed = vec.getAbs();
	if (speed > 255) speed = 255;

	for (int i = 0; i < 4; i++)
	{
		motor[i] = speed * motorMultiplier[i];
	}
}
