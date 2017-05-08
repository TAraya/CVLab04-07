#ifndef POICORRESPONDER_H
#define POICORRESPONDER_H

#include "pointofinterest.h"
#include "poiinfo.h"
#include <vector>

using namespace std;

class PoiCorresponder
{
public:
    PoiCorresponder(double nndrMax, double lMax);
    ~PoiCorresponder();

    //признак инициализации
    bool isInit() const;
    //инициализация
    void init(PoiInfo firstPoiInfo, PoiInfo secondPoiInfo);
    //получение размера первого вектора
    int firstPoiCount() const;
    //получение размера второго вектора
    int secondPoiCount() const;
    //получение элементов первого вектора
    PointOfInterest firstPoi(int index) const;
    //получение элементов второго вектора
    PointOfInterest secondPoi(int index) const;
    //получение вектора соответствий
    vector<pair<int, int>> corresponds();

private:
    bool _isInit;
    int _fstPoiCount;
    int _secPoiCount;
    vector<PointOfInterest> _firstPoiVector;
    vector<PointOfInterest> _secondPoiVector;
    vector<double> _distances;
    double _nndrMax;
    double _lMax;
    const double _epsilon = 0.000000001;
};

#endif // POICORRESPONDER_H
