#include "hystogramm.h"
#include <cmath>

Hystogramm::Hystogramm(int size)
{
    _content.resize(size);
    for (double &value: _content) value = 0.0;
    _size = size;
}

Hystogramm::~Hystogramm()
{
    ;
}

int Hystogramm::size() const
{
    return _size;
}

void Hystogramm::pushValue(double angleRad, double value)
{
    double pi = 3.14;
    //угол в диапазоне -pi .. pi приводим к 0 .. 2 * pi
    while (angleRad < 0.0) angleRad += 2.0 * pi;
    while (angleRad >= 2.0 * pi) angleRad -= 2.0 * pi;
    //расчет индексов корзин
    double doubleIndex = (double)_size * angleRad / (2.0 * pi);
    double firstCenter = std::floor(doubleIndex) + 0.5;
    double secondCenter = firstCenter;
    if (doubleIndex >= firstCenter) secondCenter += 1.0;
    else secondCenter -= 1.0;
    //интерполяция между корзинами
    double firstDistance = _abs(doubleIndex - firstCenter);
    double secondDistance = _abs(doubleIndex - secondCenter);
    int firstIndex = ((int)firstCenter + _size) % _size;
    int secondIndex = ((int)secondCenter + _size) % _size;
    //запись значений
    _content[firstIndex] += value * secondDistance;
    _content[secondIndex] += value * firstDistance;
}

double Hystogramm::getValue(double angleRad)
{
    double pi = 3.14;
    //приведение угла к диапазону 0 .. 2*pi
    while (angleRad < 0.0) angleRad += 2.0 * pi;
    while (angleRad >= 2.0 * pi) angleRad -= 2.0 * pi;
    //определение индекса в гистограмме
    double indexQuot = angleRad / (2.0 * pi);
    int index = (int)(indexQuot * (double)_size);
    //возврат результата
    return _content[index];
}

double Hystogramm::valuesSqSum()
{
    double result = 0.0;
    for (double value: _content) result += value * value;
    return result;
}

void Hystogramm::quotValues(double quot)
{
    for (double &value: _content) value *= quot;
}

void Hystogramm::trimValues(double limit)
{
    for (double &value: _content)
        if (value > limit) value = limit;
}

double Hystogramm::peekAngle(bool &hasSecondPeek, double &secondPeekAngle)
const
{
    double pi = 3.14;
    double secondPeekQuot = 0.8;
    //поиск первого пика
    int peekIndex = -1;
    double peekValue = -1.0;
    for (int index = 0; index < _size; index++)
    {
        double currValue = _content[index];
        if (peekValue <= currValue)
        {
            peekValue = currValue;
            peekIndex = index;
        }
    }
    //поиск второго пика, несмежного с основным
    int secondPeekIndex = -1;
    double secondPeekValue = -1.0;
    for (int index = 0; index < _size; index++)
    {
        if (index != peekIndex)
        {
            double currValue = _content[index];
            if (secondPeekValue <= currValue)
            {
                secondPeekValue = currValue;
                secondPeekIndex = index;
            }
        }
    }
    //соотнесение значений пиков
    hasSecondPeek = false;
    secondPeekAngle = 0.0;
    if (secondPeekValue >= secondPeekQuot * peekValue &&
        std::abs(peekIndex - secondPeekIndex) > 1)
    {
        hasSecondPeek = true;
        double leftSecValue = _content[(secondPeekIndex - 1 + _size) % _size];
        double rightSecValue = _content[(secondPeekIndex + 1) % _size];
        double secValue = _content[secondPeekIndex];
        secondPeekAngle = _interp(leftSecValue, rightSecValue, secValue,
                                  (double)(secondPeekIndex - 1) + 0.5,
                                  (double)(secondPeekIndex + 1) + 0.5,
                                  (double)secondPeekIndex + 0.5)
                * 2.0 * pi / (double)_size;
    }
    double leftValue = _content[(peekIndex - 1 + _size) % _size];
    double rightValue = _content[(peekIndex + 1) % _size];
    double value = _content[peekIndex];
    return _interp(leftValue, rightValue, value,
                   (double)(peekIndex - 1) + 0.5,
                   (double)(peekIndex + 1) + 0.5,
                   (double)peekIndex + 0.5)
                * 2.0 * pi / (double)_size;
}

double Hystogramm::_interp(double yLeft, double yRight, double y,
                           double xLeft, double xRight, double x) const
{
    //интерполируется параболой с коэффициентами a0, a1, a2
    double a2 = (yRight - yLeft) / ((xRight - xLeft) * (xRight - x)) -
                (y - yLeft) / ((x - xLeft) * (xRight - x));
    double a1 = (y - yLeft) / (x - xLeft) - a2 * (x + xLeft);
    //возврат x, в которой достигается экстремум параболы -a1/2a2
    if (_abs(a2) < _epsilon) return x;
    return -a1 / (2.0 * a2);
}

double Hystogramm::_abs(double x) const
{
    if (x < 0.0) return -1.0 * x;
    return x;
}
