#include <Eigen/Dense>
#include <cmath>

using namespace Eigen;

double abs2(Vector2d& v)
	{
		return sqrt(pow(v[0],2)+pow(v[1],2));
	}

void driveVec(Vector2d vec, float *motor)
	{
		double motorMultiplier[4] = {0, 0, 0, 0};

		motorMultiplier[0] = -vec[1];
		motorMultiplier[1] = -vec[1];
		motorMultiplier[2] = vec[1];
		motorMultiplier[3] = vec[1];

		motorMultiplier[0] += vec[0];
		motorMultiplier[1] += -vec[0];
		motorMultiplier[2] += -vec[0];
		motorMultiplier[3] += vec[0];

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

		int speed = abs2(vec);
		if (speed > 255)
			speed = 255;

		for (int i = 0; i < 4; i++)
		{
			motor[i] = speed * motorMultiplier[i];
		}
	}
