#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include "pigpio.h"

using namespace Eigen;

class Drive
{
private:
	const int ena_pin = 5;
	const int pwm_pins[4] = {26, 19, 13, 6};
	const int dir_pins[4] = {21, 20, 16, 12};

	double abs2(Vector2d &v)
	{
		return sqrt(pow(v[0],2)+pow(v[1],2));
	}

public:
	Drive()
	{
		std::cout << "Initialising Drive.\n";

		if (gpioInitialise() < 0)
		{
			std::cout << "GPIO init failed" << '\n';
		}

		// Setup ena
		gpioSetMode(ena_pin, PI_OUTPUT);
		gpioWrite(ena_pin, 0);

		// Setup pwm and dir
		for (int i = 0; i < 4; i++)
		{
			gpioSetMode(pwm_pins[i], PI_OUTPUT);
			gpioSetMode(dir_pins[i], PI_OUTPUT);
		}
	}

	void drive(Vector2d vec)
	{
		float *sMotor = new float[4];
		driveVec(vec, sMotor);

		for (int i = 0; i < 4; i++)
		{
			// std::cout << sMotor[i] << '\n';
			motor(i, sMotor[i]);
		}
	}

	// pwm 0 to 255
	void motor(int motor, int pwm)
	{
		gpioWrite(ena_pin, 1);

		if (pwm < 0)
		{
			gpioWrite(dir_pins[motor], 0);
			pwm *= -1;
		}
		else
		{
			gpioWrite(dir_pins[motor], 1);
		}

		if (pwm < 4)
			pwm = 4; //! eigentlich 25
		if (pwm > 220)
			pwm = 220; //! eigentlich 229

		std::cout << "Motor: " << motor << " PWM: " << pwm << '\n';

		gpioPWM(pwm_pins[motor], round(pwm));
	}

	void terminate()
	{
		gpioWrite(ena_pin, 0);
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
};