#ifndef HYSTOGRAMM_H
#define HYSTOGRAMM_H
#include <vector>

using namespace std;

class Hystogramm
{
public:
    Hystogramm(int hystSize);
    ~Hystogramm();

    //число корзин в гистограмме
    int size() const;
    //добавление значения
    void pushValue(double angleRad, double value);
    //извлечение значения
    double getValue(double angleRad);
    //получение суммы значений
    double valuesSqSum();
    //домножение всех значений в гистограмме на коэффициент
    void quotValues(double quot);
    //усечение всех значений в гистограмме
    void trimValues(double limit);
    //получение углов пиков
    double peekAngle(bool &hasSecondPeek, double &secondPeekAngle) const;

    friend class PointOfInterest;

private:
    int _size;
    vector<double> _content;
    double _interp(double yLeft, double yRight, double y,
                   double xLeft, double xRight, double x) const;

    double _abs(double x) const;
    double _epsilon = 0.000000001;
};

#endif // HYSTOGRAMM_H
