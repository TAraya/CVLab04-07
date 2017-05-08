#ifndef UTILS_H
#define UTILS_H
#include "enumerations.h"
#include "imagefilter.h"

//класс для вспомогательных статических методов
class Utils
{
public:
    Utils();
    ~Utils();

    static ImageFilter singleFilter(int xRadius, int yRadius);
    static ImageFilter biasFilter(int dx, int dy);

    static ImageFilter gaussFilter(double sigma);
    static ImageFilter gaussFilter(double sigma, int radius);
    static ImageFilter gaussXFilter(double sigma);
    static ImageFilter gaussXFilter(double sigma, int radius);
    static ImageFilter gaussYFilter(double sigma);
    static ImageFilter gaussYFilter(double sigma, int radius);

    static ImageFilter sobelHorizontalFilter();
    static ImageFilter sobelHorizontalXFilter();
    static ImageFilter sobelHorizontalYFilter();
    static ImageFilter sobelVerticalFilter();
    static ImageFilter sobelVerticalXFilter();
    static ImageFilter sobelVerticalYFilter();

    static double gaussWeight(double x, double y, double sigma);

    static double rotateCoords(double &x, double &y, double angleRad);

    static void coordsDeltaSerial(double angleRad,
                                  double &dxPrev, double &dyPrev,
                                  double &dxNext, double &dyNext);
    static void coordsDeltaNormal(double angleRad,
                                  double &dxLeft, double &dyLeft,
                                  double &dxRight, double &dyRight);
    static void coordsDeltaSerial(double angleRad,
                                  int &dxPrev, int &dyPrev,
                                  int &dxNext, int &dyNext);
    static void coordsDeltaNormal(double angleRad,
                                  int &dxLeft, int &dyLeft,
                                  int &dxRight, int &dyRight);

    static int directionsCount(int step);
    static void directionCoords(int index, int step, int &dx, int &dy);
};

#endif // UTILS_H
