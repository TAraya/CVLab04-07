#include "utils.h"
#include <cmath>

Utils::Utils()
{
    ;
}

Utils::~Utils()
{
    ;
}

ImageFilter Utils::singleFilter(int xRadius, int yRadius)
{
    int size = (2 * xRadius + 1) * (2 * yRadius + 1);
    vector<double> quots(size, 0.0);
    quots[xRadius + yRadius * (2 * xRadius + 1)] = 1.0;
    return {xRadius, yRadius, quots};
}

ImageFilter Utils::biasFilter(int dx, int dy)
{
    int xRadius = std::abs(dx);
    int yRadius = std::abs(dy);
    int size = (2 * xRadius + 1) * (2 * yRadius + 1);
    vector<double> quots(size, 0.0);
    quots[(xRadius + dx) + (yRadius + dy) * (2 * xRadius + 1)] = 1.0;
    return { dx, dy, quots};
}

ImageFilter Utils::gaussFilter(double sigma)
{
    int radius = std::round(3 * sigma + 0.5);
    return Utils::gaussFilter(sigma, radius);
}

ImageFilter Utils::gaussFilter(double sigma, int radius)
{
    int dimension = 2 * radius + 1;
    int size = dimension * dimension;
    double t = sigma * sigma;
    vector<double> quots(size);
    for (int y = 0; y < dimension; y++)
    {
        for (int x = 0; x < dimension; x++)
        {
            int x2 = (x - radius) * (x - radius);
            int y2 = (y - radius) * (y - radius);
            quots[x + y * dimension] =
                std::exp(-1.0 * (x2 + y2) / (2 * t)) / (2.0 * 3.14 * t);
        }
    }
    ImageFilter result(radius, radius, quots);
    result.tryNormalize();
    return result;
}

ImageFilter Utils::gaussXFilter(double sigma)
{
    int radius = std::round(3 * sigma + 0.5);
    return Utils::gaussXFilter(sigma, radius);
}

ImageFilter Utils::gaussXFilter(double sigma, int radius)
{
    int size = 2 * radius + 1;
    vector<double> quots(size);
    for (int i = 0; i < size; i++)
    {
        int x2 = (i - radius) * (i - radius);
        quots[i] = std::exp((-1.0 * x2) / (2 * sigma * sigma))
                            / (std::sqrt(2.0 * 3.14) * sigma);
    }
    ImageFilter result(radius, 0, quots);
    result.tryNormalize();
    return result;
}

ImageFilter Utils::gaussYFilter(double sigma)
{
    int radius = std::round(3 * sigma + 0.5);
    return Utils::gaussYFilter(sigma, radius);
}

ImageFilter Utils::gaussYFilter(double sigma, int radius)
{
    int size = 2 * radius + 1;
    vector<double> quots(size);
    for (int i = 0; i < size; i++)
    {
        int y2 = (i - radius) * (i - radius);
        quots[i] = std::exp((-1.0 * y2) / (2 * sigma * sigma))
                            / (std::sqrt(2.0 * 3.14) * sigma);
    }
    ImageFilter result(0, radius, quots);
    result.tryNormalize();
    return result;
}

ImageFilter Utils::sobelHorizontalFilter()
{
    vector<double> quots { -1.0,  0.0, 1.0,
                           -2.0,  0.0, 2.0,
                           -1.0,  0.0, 1.0 };
    return {1, 1, quots};
}

ImageFilter Utils::sobelHorizontalXFilter()
{
    vector<double> quots { -1.0,  0.0, 1.0 };
    return {1, 0, quots};
}

ImageFilter Utils::sobelHorizontalYFilter()
{
    vector<double> quots { 1.0,
                           2.0,
                           1.0 };
    return {0, 1, quots};
}

ImageFilter Utils::sobelVerticalFilter()
{
    vector<double> quots { -1.0, -2.0, -1.0,
                            0.0,  0.0,  0.0,
                            1.0,  2.0,  1.0};
    return {1, 1, quots};
}

ImageFilter Utils::sobelVerticalXFilter()
{
    vector<double> quots { 1.0, 2.0, 1.0 };
    return {1, 0, quots};
}

ImageFilter Utils::sobelVerticalYFilter()
{
    vector<double> quots { -1.0,
                            0.0,
                            1.0 };
    return {0, 1, quots};
}

double Utils::gaussWeight(double x, double y, double sigma)
{
    double t = sigma * sigma;
    double x2 = x * x;
    double y2 = y * y;
    return std::exp(-1.0 * (x2 + y2) / (2 * t)) / (2.0 * 3.14 * t);
}

double Utils::rotateCoords(double &x, double &y, double angleRad)
{
    double radius = std::hypot(x, y);
    double prevAngleRad = std::atan2(y, x);
    double resultAngle = prevAngleRad + angleRad;
    x = radius * std::cos(resultAngle);
    y = radius * std::sin(resultAngle);
}

void Utils::coordsDeltaSerial(double angleRad,
                              double &dxPrev, double &dyPrev,
                              double &dxNext, double &dyNext)
{
    double xDelta = std::cos(angleRad);
    double yDelta = std::sin(angleRad);
    dxPrev = -1.0 * xDelta; dyPrev = yDelta;
    dxNext = xDelta; dyNext = -1.0 * yDelta;
}

void Utils::coordsDeltaNormal(double angleRad,
                              double &dxLeft, double &dyLeft,
                              double &dxRight, double &dyRight)
{
    double xDelta = std::sin(angleRad);
    double yDelta = std::cos(angleRad);
    dxLeft = -1.0 * xDelta; dyLeft = -1.0 * yDelta;
    dxRight = xDelta; dyRight = yDelta;
}

void Utils::coordsDeltaSerial(double angleRad,
                              int &dxPrev, int &dyPrev,
                              int &dxNext, int &dyNext)
{
    double pi = 3.14;
    //определение направления
    if (angleRad < -7.0 * pi / 8.0 || angleRad >= 7.0 * pi / 8.0)
    {
        dxPrev =  1; dyPrev =  0;
        dxNext = -1; dyNext =  0;
    }
    if (angleRad >= -7.0 * pi / 8.0 && angleRad < -5.0 * pi / 8.0)
    {
        dxPrev =  1; dyPrev = -1;
        dxNext = -1; dyNext =  1;
    }
    if (angleRad >= -5.0 * pi / 8.0 && angleRad < -3.0 * pi / 8.0)
    {
        dxPrev =  0; dyPrev = -1;
        dxNext =  0; dyNext =  1;
    }
    if (angleRad >= -3.0 * pi / 8.0 && angleRad < -1.0 * pi / 8.0)
    {
        dxPrev = -1; dyPrev = -1;
        dxNext =  1; dyNext =  1;
    }
    if (angleRad >= -1.0 * pi / 8.0 && angleRad < 1.0 * pi / 8.0)
    {
        dxPrev = -1; dyPrev =  0;
        dxNext =  1; dyNext =  0;
    }
    if (angleRad >= 1.0 * pi / 8.0 && angleRad < 3.0 * pi / 8.0)
    {
        dxPrev = -1; dyPrev =  1;
        dxNext =  1; dyNext = -1;
    }
    if (angleRad >= 3.0 * pi / 8.0 && angleRad < 5.0 * pi / 8.0)
    {
        dxPrev = 0; dyPrev =  1;
        dxNext = 0; dyNext = -1;
    }
    if (angleRad >= 5.0 * pi / 8.0 && angleRad < 7.0 * pi / 8.0)
    {
        dxPrev =  1; dyPrev =  1;
        dxNext = -1; dyNext = -1;
    }
}

void Utils::coordsDeltaNormal(double angleRad,
                              int &dxLeft, int &dyLeft,
                              int &dxRight, int &dyRight)
{
    double pi = 3.14;
    //определение направления
    if (angleRad < -7.0 * pi / 8.0 || angleRad >= 7.0 * pi / 8.0)
    {
        dxLeft  =  0; dyLeft  =  1;
        dxRight =  0; dyRight = -1;
    }
    if (angleRad >= -7.0 * pi / 8.0 && angleRad < -5.0 * pi / 8.0)
    {
        dxLeft  =  1; dyLeft  =  1;
        dxRight = -1; dyRight = -1;
    }
    if (angleRad >= -5.0 * pi / 8.0 && angleRad < -3.0 * pi / 8.0)
    {
        dxLeft  =  1;  dyLeft =  0;
        dxRight = -1; dyRight =  0;
    }
    if (angleRad >= -3.0 * pi / 8.0 && angleRad < -1.0 * pi / 8.0)
    {
        dxLeft  =  1;  dyLeft = -1;
        dxRight = -1; dyRight =  1;
    }
    if (angleRad >= -1.0 * pi / 8.0 && angleRad < 1.0 * pi / 8.0)
    {
        dxLeft  =  0;  dyLeft = -1;
        dxRight =  0; dyRight =  1;
    }
    if (angleRad >= 1.0 * pi / 8.0 && angleRad < 3.0 * pi / 8.0)
    {
        dxLeft  = -1;  dyLeft = -1;
        dxRight =  1; dyRight =  1;
    }
    if (angleRad >= 3.0 * pi / 8.0 && angleRad < 5.0 * pi / 8.0)
    {
        dxLeft  = -1;  dyLeft =  0;
        dxRight =  1; dyRight =  0;
    }
    if (angleRad >= 5.0 * pi / 8.0 && angleRad < 7.0 * pi / 8.0)
    {
        dxLeft  = -1;  dyLeft =  1;
        dxRight =  1; dyRight = -1;
    }
}

int Utils::directionsCount(int step)
{
    if (step < 1) return -1;
    return 8 * step;
}

void Utils::directionCoords(int index, int step, int &dx, int &dy)
{
    /* 0--->
     * A***|
     * |*C*|
     * |***|
     * <---V
     * dx:  /--\
     *     /    \--
     *
     * dy:    /--\
     *     --/    \ */
    int dimension = 2 * step;
    dx = dy = 0;
    // в верхней части
    if (index >= 0 && index < dimension)
    {
        dx = -1 * step + index % dimension;
        dy = -1 * step;
    }
    // в правой части
    if (index >= dimension && index < 2 * dimension)
    {
        dx = step;
        dy = -1 * step + index % dimension;
    }
    // в нижней части
    if (index >= 2 * dimension && index < 3 * dimension)
    {
        dx = step - index % dimension;
        dy = step;
    }
    // в левой части
    if (index >= 3 * dimension && index < 4 * dimension)
    {
        dx = -1 * step;
        dy = step - index % dimension;
    }
}
