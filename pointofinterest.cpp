#include "pointofinterest.h"
#include "imagefilter.h"
#include "utils.h"
#include <cmath>

PointOfInterest::PointOfInterest()
{
    _x = _y = _value = _peekAngle = _scale = 0.0;
    _coordQuot = 1;
    _isDescInit = false;
    _hystCount = 0;
}

PointOfInterest::PointOfInterest(double x, double y, double value,
                                 double peekAngle, double scale,
                                 double coordQuot)
{
    _x = x;
    _y = y;
    _value = value;
    _scale = scale;
    _coordQuot = coordQuot;
    _peekAngle = peekAngle;
    _isDescInit = false;
    _hystCount = 0;
}

PointOfInterest::~PointOfInterest()
{
    ;
}

double PointOfInterest::x() const
{
    return _x;
}

double PointOfInterest::y() const
{
    return _y;
}

double PointOfInterest::value() const
{
    return _value;
}

double PointOfInterest::scale() const
{
    return _scale;
}

double PointOfInterest::coordQuot() const
{
    return _coordQuot;
}

double PointOfInterest::peekAngle() const
{
    return _peekAngle;
}

void PointOfInterest::setPeekAngle(double value)
{
    _peekAngle = value;
}

bool PointOfInterest::isDescInit() const
{
    return _isDescInit;
}

void PointOfInterest::initDesc(int netSize, int hystSize)
{
    _hystCount = netSize * netSize;
    _descriptor._hystContent.clear();
    for (int index = 0; index < _hystCount; index++)
    {
        Hystogramm hyst(hystSize);
        _descriptor._hystContent.push_back(hyst);
        _descriptor._hystCount++;
    }
    _isDescInit = true;
}

void PointOfInterest::calcDesc(const SubImage &image, int radius,
                               double maskSigma, bool consRotate)
{
    if (!_isDescInit) return ;

    /* -r       0    r
     *  0  1 I  2  3
     *  4  5 I  6  7
     * ------+------
     *  8  9 I 10 11
     * 12 13 I 14 15
     *
     * разделение не по пикселам, а между ними */

    int netSize = std::sqrt(_hystCount);

    //координаты центра сетки
    int xCenter = (int)_x;
    int yCenter = (int)_y;

    for (int dy = -1 * radius; dy < radius; dy++)
    {
        for (int dx = -1 * radius; dx < radius; dx++)
        {
            //расчет упрощенных (-1 0 1) градиентов
            double hGrad, vGrad;
            hGrad = image.pixel(xCenter + 1 + dx, (int)yCenter + dy) -
                    image.pixel(xCenter - 1 + dx, (int)yCenter + dy);
            vGrad = image.pixel(xCenter + dx, (int)yCenter + 1 + dy) -
                    image.pixel(xCenter + dx, (int)yCenter - 1 + dy);

            //расчет угла и магнитуды
            double angle = std::atan2(vGrad, hGrad);
            double value = std::hypot(hGrad, vGrad);

            //координаты в дескрипторе
            double dxDesc = dx + 0.5;
            double dyDesc = dy + 0.5;

            //учет ориентации
            if (consRotate)
            {
                //учет ориентации в координатах (поворот на обратный угол пика)
                Utils::rotateCoords(dxDesc, dyDesc, -1.0 * _peekAngle);
                //учет ориентации в градиентах (поворот на обратный угол пика)
                angle -= _peekAngle;
            }

            //расчет индексов ячеек (гистограмм)
            int xFstIndex, yFstIndex, xSecIndex, ySecIndex;
            double xQuot = (double)(dxDesc + radius) / (2.0 * (double)radius);
            double yQuot = (double)(dyDesc + radius) / (2.0 * (double)radius);
            xFstIndex = (int)(xQuot * (double)netSize);
            yFstIndex = (int)(yQuot * (double)netSize);

            //расчет центров 4 ближайщих ячеек (гистограмм) сетки
            double cellLength = (2.0 * (double)radius) / (double)netSize;
            double xCellFst = -1.0 * radius + cellLength * (xFstIndex + 0.5);
            double yCellFst = -1.0 * radius + cellLength * (yFstIndex + 0.5);
            double xCellSec = xCellFst;
            double yCellSec = yCellFst;
            if (dxDesc < xCellFst)
            {
                xCellSec -= cellLength;
                xSecIndex = xFstIndex - 1;
            }
            else
            {
                xCellSec += cellLength;
                xSecIndex = xFstIndex + 1;
            }
            if (dyDesc < yCellFst)
            {
                yCellSec -= cellLength;
                ySecIndex = yFstIndex - 1;
            }
            else
            {
                yCellSec += cellLength;
                ySecIndex = yFstIndex + 1;
            }

            //после поворота коорд. возможен выход за [-radius ... radius - 1]
            //такие ячейки отбрасываются
            if (dxDesc < -1.0 * (double)radius || dxDesc > (double)radius ||
                dyDesc < -1.0 * (double)radius || dyDesc > (double)radius)
                continue;

            if (xFstIndex < 0) xFstIndex = 0;
            if (xFstIndex >= netSize) xFstIndex = netSize - 1;
            if (yFstIndex < 0) yFstIndex = 0;
            if (yFstIndex >= netSize) yFstIndex = netSize - 1;
            if (xSecIndex < 0) xSecIndex = 0;
            if (xSecIndex >= netSize) xSecIndex = netSize - 1;
            if (ySecIndex < 0) ySecIndex = 0;
            if (ySecIndex >= netSize) ySecIndex = netSize - 1;

            //интерполяция к центрам ячеек (гистограмм)
            double quotXFst = (dxDesc - xCellSec) / cellLength;
            double quotXSec = (dxDesc - xCellFst) / cellLength;
            double quotYFst = (dyDesc - yCellSec) / cellLength;
            double quotYSec = (dyDesc - yCellFst) / cellLength;
            if (quotXFst < 0.0) quotXFst *= -1.0;
            if (quotXSec < 0.0) quotXSec *= -1.0;
            if (quotYFst < 0.0) quotYFst *= -1.0;
            if (quotYSec < 0.0) quotYSec *= -1.0;
            //взвешивание значения магнитуды по Гауссу
            double quot = Utils::gaussWeight(dxDesc, dyDesc, maskSigma);

            //помещение в гистограммы (интерполяция автоматически)
            _descriptor._hystContent[xFstIndex + yFstIndex * netSize].
                    pushValue(angle, quot * quotXFst * quotYFst * value);
            _descriptor._hystContent[xSecIndex + yFstIndex * netSize].
                    pushValue(angle, quot * quotXSec * quotYFst * value);
            _descriptor._hystContent[xFstIndex + ySecIndex * netSize].
                    pushValue(angle, quot * quotXFst * quotYSec * value);
            _descriptor._hystContent[xSecIndex + ySecIndex * netSize].
                    pushValue(angle, quot * quotXSec * quotYSec * value);
        }
    }
}

double PointOfInterest::calcPeekAngle(const SubImage &image, int binCount,
                                      int radius, double maskSigma,
                                      bool &hasSecondPeek,
                                      double &secondPeekAngle)
const
{
    Hystogramm hyst(binCount);
    ImageFilter mask = Utils::gaussFilter(maskSigma, radius);

    for (int dy = -1 * radius; dy <= radius; dy++)
    {
        for (int dx = -1 * radius; dx <= radius; dx++)
        {
            double hGrad = image.pixel(_x + dx + 1, _y + dy) -
                                image.pixel(_x + dx - 1, _y + dy);
            double vGrad = image.pixel(_x + dx, _y + dy + 1) -
                                image.pixel(_x + dx, _y + dy - 1);
            double magnitude = std::sqrt(hGrad * hGrad + vGrad * vGrad);
            double angle = std::atan2(vGrad, hGrad);
            double quot = mask.quot(dx, dy);
            hyst.pushValue(angle, magnitude * quot);
        }
    }
    return hyst.peekAngle(hasSecondPeek, secondPeekAngle);
}

void PointOfInterest::normalizeDescSingle()
{
    if (!_isDescInit) return ;
    _descriptor.normalizeSingle();
}

void PointOfInterest::normalizeDescComplex()
{
    if (!_isDescInit) return ;
    _descriptor.normalizeComplex();
}

void PointOfInterest::normalizeDescTrimComplex()
{
    if (!_isDescInit) return ;
    _descriptor.normalizeTrimComplex(0.2);
}

double PointOfInterest::calcDistance(PointOfInterest &firstPoi,
                                     PointOfInterest &secondPoi)
{
    if (!firstPoi._isDescInit || !secondPoi._isDescInit) return -1.0;
    if (firstPoi._hystCount != secondPoi._hystCount) return -1.0;

    //Эвклидова метрика
    int hystCount = firstPoi._hystCount;
    int hystSize = firstPoi._descriptor._hystContent[0].size();
    double result = 0.0;
    for (int hystIndex = 0; hystIndex < hystCount; hystIndex++)
        for (int binIndex = 0; binIndex < hystSize; binIndex++)
        {
            double diff = (firstPoi._descriptor._hystContent[hystIndex]._content[binIndex] -
                           secondPoi._descriptor._hystContent[hystIndex]._content[binIndex]);
            result += diff * diff;
        }
    return std::sqrt(result);
}
