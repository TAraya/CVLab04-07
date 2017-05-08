#ifndef POINTOFINTEREST_H
#define POINTOFINTEREST_H
#include "basedescriptor.h"

class PointOfInterest
{
public:
    PointOfInterest();
    PointOfInterest(double x, double y, double value = 1.0,
                    double peekAngle = 0.0, double scale = 0.0,
                    double coordQuot = 1.0);
    ~PointOfInterest();

    double x() const;
    double y() const;
    double value() const;
    double scale() const;
    double coordQuot() const;
    double peekAngle() const;
    void setPeekAngle(double value);
    //признак инициализаци дескриптора
    bool isDescInit() const;
    //инициализация дескриптора
    void initDesc(int netSize, int hystSize);
    //простой расчет дескриптора по сетке
    void calcDesc(const SubImage &image, int radius, double maskSigma,
                  bool consRotate = true);
    //поиск угла соотв. наибольшему направлению градиента
    double calcPeekAngle(const SubImage &image, int binCount,
                         int radius, double maskSigma,
                         bool &hasSecondPeek, double &secondPeekAngle) const;
    //нормализация дескриптора
    void normalizeDescSingle();
    void normalizeDescComplex();
    void normalizeDescTrimComplex();
    //расчет растояний между дескрипторами точки интереса
    static double calcDistance(PointOfInterest &firstPoi,
                               PointOfInterest &secondPoi);

private:
    double _x;
    double _y;
    double _value;
    double _peekAngle;
    double _scale;
    double _coordQuot;
    bool _isDescInit;
    BaseDescriptor _descriptor;
    int _hystCount;
    double _epsilon = 0.000000001;
};

#endif // POINTOFINTEREST_H
