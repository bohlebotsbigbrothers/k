#include <cmath>

struct Vector2
{
    double x, y;

    // outputs
    double getRot()
    {
        return (atan2(y, x)); //-179l to 180  left to right
    }
    double getAbs()
    {
        return (sqrt(pow(x, 2) + pow(y, 2))); // returns the length of the vector
    }
    void getNormalized()
    {
        double abs = getAbs();
        x/=abs;
        y/=abs;
    }

    // manipulation
    void norm()
    {
        if (x >= y)
        {
            y = y / x;
            x = 1;
        }
        else
        {
            x = x / y;
            y = 1;
        }
    }
    void setRot(double r) //-179 to 180  left to right //set rotation
    {
        int abs = getAbs();
        x = sin(r) * abs;
        y = cos(r) * abs;
    }
    void rot(double r) // change rotation by set value
    {
        int rr = getRot() + r;
        int abs = getAbs();
        x = sin(rr) * abs;
        y = cos(rr) * abs;
    }
    void setAbs(double abs)
    {
        int rot = getRot();
        x = sin(rot) * abs;
        y = cos(rot) * abs;
    }

    // vector calc
    void addVec(Vector2 v)
    {
        x += v.x;
        y += v.y;
    }
    void setToVec(Vector2 v)
    {
        x = v.x;
        y = v.y;
    }
};