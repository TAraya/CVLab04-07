#ifndef IMAGEBUILDER_H
#define IMAGEBUILDER_H

#include "poiinfo.h"
#include "enumerations.h"
#include "imagetransformer.h"
#include "pointofinterest.h"
#include "poicorresponder.h"
#include "subimage.h"
#include "utils.h"

//предоставляет реализации с визуализацией различных операторов
class ImageBuilder
{
public:
    ImageBuilder();
    ~ImageBuilder();
    //возвращает визуализацию точек интереса на изображении
    static SubImage buildImageWithPoi(SubImage &source,
                                      vector<PointOfInterest> &poiVector);
    //возвращает визуализацию блобов в изображении
    static SubImage buildImageWithBlob(SubImage &source,
                                       vector<PointOfInterest> &poiVector,
                                       int radius, double initSigma);
    //возвращает два изображения в едином представлении
    static SubImage buildTwinImage(SubImage &left, SubImage &right);
    //возвращает два изобр. в едином представлении с соотв. точками интереса
    static SubImage buildTwinImage(SubImage &left, SubImage &right,
                                   PoiCorresponder &matcher, bool fill,
                                   int radius = 0, double initSigma = 0.0);

private:
    //отрисовывает точку c координатами x,y и радиусом radius на изображении
    static void _drawCircle(SubImage &image,
                            int x, int y, double radius, bool fill);
    //отрисовывает линию между точками
    static void _drawLine(SubImage &image, int xFirst, int yFirst,
                          int xSecond, int ySecond);
    //коэффициент для радиуса точки интереса при отрисовке
    static const double _drawQuot;
};

#endif // IMAGEBUILDER_H
