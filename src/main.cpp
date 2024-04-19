#include <iostream>
#include <thread>
#include <unistd.h>
#include <Eigen/Dense>
#include "pigpio.h"
#include "./i2c.h"
#include "./drive.h"

#define PI 3.141

using namespace Eigen;

bool stop = false;

void loop(Input_i2c i2c, Drive drive)
{
        i2c.readIr();
        std::cout << "Sees a ball: " << i2c.ball_exists << '\n';
        std::cout << "Ball distance: " << i2c.ball_distance << '\n';
        std::cout << "Ball dir: " << i2c.ball_direction << '\n' << '\n' << '\n';

        i2c.readGroundSensor();
        std::cout << "LineSeen: " << i2c.lineSeen << '\n';
        std::cout << "LineDir: " << i2c.lineDir << '\n';
        std::cout << "LineFlag" << i2c.lineFlag << '\n';
        std::cout << "LineUnique" << i2c.lineUnique << '\n' << '\n' << '\n';

        double a = double(i2c.ball_direction)/8*PI;
	    a = a*1.1;
        Vector2d ball(cos(a), sin(a));

        if (i2c.lineSeen==true)
        {
            double b = double(i2c.lineDir)/8*PI;
            Vector2d line(cos(b), sin(a));

            ball = -line;
        }

        ball = ball * 100;

        float* motor = nullptr;
        drive.drive(ball);

        usleep(10000);
}

int main()
{
    if (gpioInitialise() < 0)
    {
        std::cerr << "Failed to initialise pigpio.\n";
    }

    Input_i2c i2c;
    Drive drive;

    while(i2c.buttonPressed(1,1)==false) 
    {
	i2c.setLed(0,0,1);
	while(i2c.buttonPressed(0,0)==false)
	{
	    usleep(10000);
	}
	while(i2c.buttonPressed(0,0)==true){usleep(10000);}
	i2c.setLed(0,0,0);
	while(i2c.buttonPressed(0,0)==false)
	{
	    loop(i2c, drive);
	}
	drive.stop();
	while(i2c.buttonPressed(0,0)==true){usleep(10000);}
	//i2c.setLed(1,0,0);
	std::cout << "Terminating.\n";
    }
    drive.terminate();
    i2c.closeI2c();
    gpioTerminate();

    return 0;
}
