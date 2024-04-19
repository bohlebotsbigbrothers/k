#include <iostream>
#include "pigpio.h"

enum Color
{
    NONE,
    GREEN,
    RED,
    YELLOW,
    BLUE,
    CYAN,
    MAGENTA,
    WHITE
};

/* 
 * Input i2c class
 * gpioInitialise() and gpioTerminate() must be called before and after initialisation/termination
 */ 
class Input_i2c
{
private:
    const int i2c_pins[2] = {2, 3};
    const int button_devices[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
    const int ground_sensor = 0x30;
    const int compass = 0x60;
    const int ir_ring = 0x55;
    int led0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int led1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int handles[8] = {0, 0, 0, 0, 0, 0, 0, 0};

public:
    bool lineSeen;
    bool lineFlag;
    bool lineUnique;
    int lineDir;

    int ball_direction;
    int ball_distance;
    bool ball_exists;

    Input_i2c()
    {
        // setup i2c pins
        gpioSetMode(i2c_pins[0], PI_ALT0);
        gpioSetMode(i2c_pins[1], PI_ALT0);

        // set i2c handles
        handles[0] = i2cOpen(1, button_devices[0], 0);
        handles[1] = i2cOpen(1, button_devices[1], 0);

        // ground sensor
        handles[2] = i2cOpen(1, ground_sensor, 0);

        // compass
        handles[3] = i2cOpen(1, compass, 0);

        // ring
        handles[4] = i2cOpen(1, ir_ring, 0);

	setLed(0,0,0);
	setLed(0,1,0);
	setLed(1,0,0);
	setLed(1,1,0);

        std::cout << "I2c input class initialised." << std::endl;
    }

    void readIr()
    {
        int ir_packet = i2cReadByte(handles[4]);
        this->ball_direction = (ir_packet % 16) - 7;
        this->ball_distance = ir_packet / 16;
        this->ball_exists = (this->ball_distance != 0);
    }

    void setLed(int device, int led_number, int color)
    {
        if (led_number == 0)
        {
            color *= 2;
            led0[device] = color;
        }
        if (led_number == 1)
        {
            color *= 16;
            if (color > 63)
                color += 64;
            led1[device] = color;
        }

        // send i2c
        i2cWriteByte(handles[device], 255 - led0[device] - led1[device]);
    }

    bool buttonPressed(int device, int btn_number)
    {
        int button_read = i2cReadByte(handles[device]);
        std::cout << "Button read: " << button_read << std::endl;
        if(btn_number == 0)
        {
            return button_read == (255 - led0[device] - led1[device] - 1);
        }
        if(btn_number == 1)
        {
            std::cout << "Debug: " << (255 - led0[device] - led1[device] - 64) << "\n";
            return button_read == (255 - led0[device] - led1[device] - 64);
        }
        return 0;
    }

    int readGroundSensor()
    {
        uint8_t groundpacket = i2cReadByte(handles[2]);

        lineSeen = groundpacket & 64;
        lineFlag = groundpacket & 32;
        lineUnique = groundpacket & 16;
        lineDir = (groundpacket % 16) - 7;

        return 0;
    }

    // random movements
    void calibrateCompass()
    {
        std::cout << "Calibrating..." << std::endl;
        char buffer[1];

        buffer[0] = 0x98;
        i2cWriteI2CBlockData(handles[3], 0x00, buffer, 1);

        gpioDelay(20000);

        buffer[0] = 0x95;
        i2cWriteI2CBlockData(handles[3], 0x00, buffer, 1);

        gpioDelay(20000);

        buffer[0] = 0x99;
        i2cWriteI2CBlockData(handles[3], 0x00, buffer, 1);

        gpioDelay(20000);

        buffer[0] = 0b10000100;
        i2cWriteI2CBlockData(handles[3], 0x00, buffer, 1);

        for (int i = 20; i > 0; i--)
        {
            std::cout << i << std::endl;
            gpioDelay(1000000);
        }

        this->calibrationStore();
        this->readCalibration();
    }

    void calibrationStore()
    {
        std::cout << "Storing..." << std::endl;
        char buffer[1];

        buffer[0] = 0xF0;
        i2cWriteI2CBlockData(handles[3], 0x00, buffer, 1);

        gpioDelay(20000);

        buffer[0] = 0xF5;
        i2cWriteI2CBlockData(handles[3], 0x00, buffer, 1);

        gpioDelay(20000);

        buffer[0] = 0xF6;
        i2cWriteI2CBlockData(handles[3], 0x00, buffer, 1);

        gpioDelay(20000);
    }

    void readCompass()
    {
        char buffer[0];
        while (true)
        {
            uint8_t compasspacket = i2cReadI2CBlockData(handles[3], 0x01, buffer, 1); // i2cReadBlockData not working /:

            // int compass = compasspacket * 256 + compasspacket2;

            std::cout << "Compass: " << static_cast<int>(buffer[0]);
            std::cout << "\n";

            gpioDelay(2000);
        }
    }

    void readCalibration()
    {
        char buffer[0];
        i2cReadI2CBlockData(handles[3], 0x1E, buffer, 1); // i2cReadBlockData not working /:

        std::cout << "Calibration: " << static_cast<int>(buffer[0]);
        std::cout << "\n";

        gpioDelay(2000);
    }

    void closeI2c()
    {
        std::cout << "Closing i2c handles\n";
        i2cClose(handles[0]);
        i2cClose(handles[1]);
        i2cClose(handles[2]);
        i2cClose(handles[3]);
        i2cClose(handles[4]);
    }
};
