#include "basedescriptor.h"
#include "imagefilter.h"
#include "utils.h"
#include <cmath>

BaseDescriptor::BaseDescriptor()
{
    _hystCount = 0;
    _hystContent.clear();
}

BaseDescriptor::~BaseDescriptor()
{
    ;
}

int BaseDescriptor::hystCount() const
{
    return _hystCount;
}

void BaseDescriptor::normalizeSingle()
{
    //длина каждой гистограмма (вектора) приводится к 1.0
    double average = 1.0;
    for (int hystIndex = 0; hystIndex < _hystCount; hystIndex++)
    {
        double valuesSqSum = _hystContent[hystIndex].valuesSqSum();
        if (valuesSqSum < _epsilon) valuesSqSum = 1.0;
        double quot = 1.0 / std::sqrt(valuesSqSum);
        _hystContent[hystIndex].quotValues(quot);
    }
}

void BaseDescriptor::normalizeComplex()
{
    //длина вектора корзин всех гистограмм приводится к 1.0
    double valuesSqSum = 0.0;
    for (int hystIndex = 0; hystIndex < _hystCount; hystIndex++)
        valuesSqSum += _hystContent[hystIndex].valuesSqSum();
    if (valuesSqSum < _epsilon) valuesSqSum = 1.0;
    double quot = 1.0 / std::sqrt(valuesSqSum);
    for (int hystIndex = 0; hystIndex < _hystCount; hystIndex++)
        _hystContent[hystIndex].quotValues(quot);
}

void BaseDescriptor::normalizeTrimComplex(double limit)
{
    //сумма значений всех гистограмм приводится к 1.0
    normalizeComplex();
    //все значения в гистограмме усекаются по limit
    for (int hystIndex = 0; hystIndex < _hystCount; hystIndex++)
        _hystContent[hystIndex].trimValues(limit);
    //сумма значений всех гистограмм снова приводится к 1.0
    normalizeComplex();
}
