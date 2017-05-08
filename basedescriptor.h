#ifndef BASEDESCRIPTOR_H
#define BASEDESCRIPTOR_H
#include "hystogramm.h"
#include "subimage.h"
#include "enumerations.h"
#include <vector>

class BaseDescriptor
{
public:
    BaseDescriptor();
    ~BaseDescriptor();

    //число хранимых гистограмм
    int hystCount() const;
    //раздельная нормализация дескриптора
    void normalizeSingle();
    //комплексаная нормализация дескриптора
    void normalizeComplex();
    //комплексная нормализация дескриптора с отсечением
    void normalizeTrimComplex(double limit);

    friend class PointOfInterest;

private:
    int _hystCount;
    vector<Hystogramm> _hystContent;
    double _epsilon = 0.000000001;

};

#endif // BASEDESCRIPTOR_H
