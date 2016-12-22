#pragma once
struct Point3D
{
    double x;
    double y;
    double z;

    Point3D(double _x = 0.0, double _y = 0.0, double _z = 0.0) :x(_x), y(_y), z(_z)
    {
    }

    Point3D& operator=(const Point3D &rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }
};
