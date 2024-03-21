#include <iostream>
#include "pigpio.h"
#include "vec_drive.h"

class Drive
{
private:
    const int ena_pin = 5;
    const int pwm_pins[4] = {26, 19, 13, 6};
    const int dir_pins[4] = {21, 20, 16, 12};

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

    void drive(Vector2 vec)
    {
        float* sMotor = new float[4];
        driveVec(vec, sMotor);
        
        for (int i = 0; i < 4; i++)
        {
            std::cout << sMotor[i] << '\n';
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

        if(pwm < 4) pwm = 4; //! eigentlich 25
        if(pwm > 220) pwm = 220; //! eigentlich 229

        std::cout << "Motor: " << motor << " PWM: " << pwm << '\n';
        gpioPWM(pwm_pins[motor], pwm);
    }

    void terminate()
    {
        gpioWrite(ena_pin, 0);
        gpioTerminate();
    }
};

int main()
{
    Drive drive;

    // create vec from console input
    Vector2 vec;
    std::cin >> vec.x >> vec.y;

    drive.drive(vec);
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard remaining characters in the input buffer
    std::cin.get();

    drive.terminate();
    return 0;
}