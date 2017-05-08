#include "imagefilter.h"
#include <cmath>

ImageFilter::ImageFilter()
{
    _xRadius = _yRadius = -1;
}

ImageFilter::ImageFilter(int xRadius, int yRadius,
                         const vector<double> &quots)
{
    _xRadius = xRadius;
    _yRadius = yRadius;
    _quots = quots;
}

ImageFilter::~ImageFilter()
{
    ;
}

int ImageFilter::xRadius() const
{
    return _xRadius;
}

int ImageFilter::yRadius() const
{
    return _yRadius;
}

int ImageFilter::width() const
{
    return 2 * _xRadius + 1;
}

int ImageFilter::height() const
{
    return 2 * _yRadius + 1;
}

double ImageFilter::quot(int dx, int dy) const
{
    //контроль индексов
    if (dx < -1 * _xRadius || dx > _xRadius ||
        dy < -1 * _yRadius || dy > _yRadius)
        return 0.0;
    int index = (_xRadius + dx) + (_yRadius + dy) * width();
    return _quots[index];
}

void ImageFilter::tryNormalize()
{
    double quotsSum = 0.0;
    for (double value: _quots) quotsSum += value;
    if (std::abs(quotsSum) > 0.000000001)
        for (double &value: _quots) value /= quotsSum;
}


