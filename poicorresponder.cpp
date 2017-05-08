#include "poicorresponder.h"
#include "pointofinterest.h"
#include "poiinfo.h"

PoiCorresponder::PoiCorresponder(double nndrMax, double lMax)
{
    _isInit = false;
    _fstPoiCount = _secPoiCount = -1;
    _nndrMax = nndrMax;
    _lMax = lMax;
}

PoiCorresponder::~PoiCorresponder()
{
    ;
}

bool PoiCorresponder::isInit() const
{
    return _isInit;
}

void PoiCorresponder::init(PoiInfo firstPoiInfo, PoiInfo secondPoiInfo)
{
    //получение точек интереса
    _firstPoiVector =
            firstPoiInfo.poiVector(-1, _fstPoiCount);
    _secondPoiVector =
            secondPoiInfo.poiVector(-1, _secPoiCount);

    //инициализация матрицы расстояний между точками интереса
    _distances.resize(_fstPoiCount * _secPoiCount);
    for (double &value: _distances) value = 0.0;

    //расчет растояний между точками интереса
    for (int firstIndex = 0; firstIndex < _fstPoiCount; firstIndex++)
        for (int secondIndex = 0; secondIndex < _secPoiCount; secondIndex++)
            _distances[firstIndex + secondIndex * _fstPoiCount] =
                PointOfInterest::calcDistance(_firstPoiVector[firstIndex],
                                              _secondPoiVector[secondIndex]);
    _isInit = true;
}

int PoiCorresponder::firstPoiCount() const
{
    if (!_isInit) return -1;

    return _fstPoiCount;
}

int PoiCorresponder::secondPoiCount() const
{
    if (!_isInit) return -1;

    return _secPoiCount;
}

PointOfInterest PoiCorresponder::firstPoi(int index) const
{
    if (!_isInit) return PointOfInterest();

    return _firstPoiVector[index];
}

PointOfInterest PoiCorresponder::secondPoi(int index) const
{
    if (!_isInit) return PointOfInterest();

    return _secondPoiVector[index];
}

vector<pair<int, int>> PoiCorresponder::corresponds()
{
    if (!_isInit) return vector<pair<int, int>>(0);

    vector<pair<int, int>> result(0);
    //маска для сопоставления
    vector<bool> consMask(_fstPoiCount * _secPoiCount, true);
    //проверка l < lMax
    for (int fstIndex = 0; fstIndex < _fstPoiCount; fstIndex++)
        for (int secIndex = 0; secIndex < _secPoiCount; secIndex++)
            if (_distances[fstIndex + secIndex * _fstPoiCount] > _lMax)
                consMask[fstIndex + secIndex * _fstPoiCount] = false;
    //поиск соответствий
    bool endFlag = false;
    while (!endFlag)
    {
        //поиск наилучшего соответствия с учетом маски
        double bestValue = _lMax + _epsilon;
        int bestFstIndex = -1;
        int bestSecIndex = -1;
        for (int fstIndex = 0; fstIndex < _fstPoiCount; fstIndex++)
        {
            for (int secIndex = 0; secIndex < _secPoiCount; secIndex++)
            {
                double value = _distances[fstIndex + secIndex * _fstPoiCount];
                if (value <= bestValue &&
                        consMask[fstIndex + secIndex * _fstPoiCount])
                {
                    bestValue = value;
                    bestFstIndex = fstIndex;
                    bestSecIndex = secIndex;
                }
            }
        }
        if (bestFstIndex > -1 && bestSecIndex > -1)
        {
            //поиск ближайшего соответствия
            double nextValue = _lMax + _epsilon;
            int nextIndex = -1;
            for (int index = 0; index < _secPoiCount; index++)
            {
                double value = _distances[bestFstIndex + index * _fstPoiCount];
                //рассматриваются в т.ч. и исключенные точки
                if (index != bestSecIndex && value <= nextValue)
                {
                    nextValue = value;
                    nextIndex = index;
                }
            }
            //подсчет nndr
            double nndr = 0.0;
            if (nextIndex > -1) nndr = bestValue / nextValue;
            if (nndr <= _nndrMax)
            {
                //соответствие признается верным
                result.push_back(pair<int, int>(bestFstIndex, bestSecIndex));
                //отметки о соответствии
                for (int index = 0; index < _fstPoiCount; index++)
                    consMask[index + bestSecIndex * _fstPoiCount] = false;
                for (int index = 0; index < _secPoiCount; index++)
                    consMask[bestFstIndex + index * _fstPoiCount] = false;
            }
            else
            {
                //соответствия отбрасываются
                consMask[bestFstIndex + bestSecIndex * _fstPoiCount] = false;
                consMask[bestFstIndex + nextIndex * _fstPoiCount] = false;
            }
        }
        else endFlag = true; //не осталось ни одного соответствия
    }
    //возврат результата
    return result;
}
