#include <stdio.h>
#include <iostream>
#include <pigpio.h>

#define DRIVE_ENA 5

#define DRIVE1_DIR 20
#define DRIVE2_DIR 21
#define DRIVE3_DIR 16
#define DRIVE4_DIR 12

#define DRIVE1_PWM 19
#define DRIVE2_PWM 26
#define DRIVE3_PWM 13
#define DRIVE4_PWM 6

// LEJS drive algorith
class Drive_LEJS
{
public:
    Drive_LEJS()
    {
        std::cout << "Drive_LEJS initialised." << '\n';
        gpioSetMode(DRIVE_ENA, PI_OUTPUT);
        gpioWrite(DRIVE_ENA, 0); //=ena
        gpioSetMode(DRIVE1_DIR, PI_OUTPUT);
        gpioSetMode(DRIVE2_DIR, PI_OUTPUT);
        gpioSetMode(DRIVE3_DIR, PI_OUTPUT);
        gpioSetMode(DRIVE4_DIR, PI_OUTPUT);

        gpioSetMode(DRIVE1_PWM, PI_OUTPUT);
        gpioSetMode(DRIVE2_PWM, PI_OUTPUT);
        gpioSetMode(DRIVE3_PWM, PI_OUTPUT);
        gpioSetMode(DRIVE4_PWM, PI_OUTPUT);
    }
    int spdToPWM(int speed) //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        if (speed < 0)
            speed *= -1;
        return (25 + (speed * 205) / 100);
    }
    void motor(int number, int speed)
    {
        gpioWrite(DRIVE_ENA, 1);
        // Speed wird bei 100 und -100 gekappt
        if (speed > 100)
            speed = 100;
        if (speed < -100)
            speed = -100;
        int pwm = spdToPWM(speed);
        int dir;
        if (speed < 0)
            dir = 0;
        else
            dir = 1;

        if (number == 1)
            gpioWrite(DRIVE1_DIR, dir);
        if (number == 2)
            gpioWrite(DRIVE2_DIR, dir);
        if (number == 3)
            gpioWrite(DRIVE3_DIR, dir);
        if (number == 4)
            gpioWrite(DRIVE4_DIR, dir);

        std::cout << "PWM: " << pwm << '\n';
        if (number == 1) gpioPWM(DRIVE1_PWM, pwm);
        if (number == 2) gpioPWM(DRIVE2_PWM, pwm);
        if (number == 3) gpioPWM(DRIVE3_PWM, pwm);
        if (number == 4) gpioPWM(DRIVE4_PWM, pwm);

    }

    void fahre(int direction, int speed, int rotation)
    {
        if (direction == 0)
        { // gradeaus
            motor(1, speed + rotation);
            motor(2, -speed + rotation);
            motor(3, speed + rotation);
            motor(4, -speed + rotation);
        }
        if (direction == 1)
        { // 45 Grad rechts
            motor(1, speed + rotation);
            motor(2, rotation);
            motor(3, rotation);
            motor(4, -speed + rotation);
        }
        if (direction == 2)
        { // rechts
            motor(1, speed + rotation);
            motor(2, speed + rotation);
            motor(3, -speed + rotation);
            motor(4, -speed + rotation);
        }
        if (direction == 3)
        { // 135 Grad rechts
            motor(1, rotation);
            motor(2, speed + rotation);
            motor(3, -speed + rotation);
            motor(4, rotation);
        }

        if (direction == 4)
        { // zurück
            motor(1, -speed + rotation);
            motor(2, speed + rotation);
            motor(3, -speed + rotation);
            motor(4, speed + rotation);
        }

        if (direction == -1)
        { // 45 Grad links
            motor(1, rotation);
            motor(2, -speed + rotation);
            motor(3, speed + rotation);
            motor(4, rotation);
        }

        if (direction == -2)
        { // links

            motor(1, -speed + rotation);
            motor(2, -speed + rotation);
            motor(3, speed + rotation);
            motor(4, speed + rotation);
        }

        if (direction == -3)
        { // 135 Grad links
            motor(1, -speed + rotation);
            motor(2, rotation);
            motor(3, rotation);
            motor(4, speed + rotation);
        }
    }
};

class Drive_k
{

}

#include <iostream>
#include <conio.h> // for _getch() function

int main()
{
    if (gpioInitialise() < 0)
    {
        std::cout << "GPIO init failed" << '\n';
        return 1;
    }

    Drive_LEJS drive;
    drive.motorenSetup();

    bool driving = true;
    while (driving)
    {
        char key = _getch(); // get a key from the user

        switch (key)
        {
            case 'w':
                drive.fahre(0, 20, 0); // drive forward
                break;
            case 'a':
                drive.fahre(-2, 20, 0); // turn left
                break;
            case 's':
                drive.fahre(4, 20, 0); // drive backward
                break;
            case 'd':
                drive.fahre(2, 20, 0); // turn right
                break;
            case 'p': // break when 'p' key is pressed
                driving = false; // stop driving
                break;
        }
    }

    drive.fahre(0, 0, 0); // stop driving

    std::cout << "GPIO terminated" << '\n';
    gpioWrite(DRIVE_ENA, 0);
    gpioTerminate();

    return 0;
}