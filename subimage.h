#ifndef SUBIMAGE_H
#define SUBIMAGE_H

#include "enumerations.h"
#include <QImage>

using namespace std;

//хранит представление изображения
class SubImage
{
public:
    SubImage();
    SubImage(const QImage &image);
    SubImage(int width, int height);
    ~SubImage();
    //инициализирует изображение в соответствии с QImage
    void loadImage(const QImage &image);
    //возвращает изображение QImage
    QImage getQImage() const;
    //возвращает ширину изображения
    int width() const;
    //возвращает высоту изображения
    int height() const;
    //возвращает произвольный пиксель (в т.ч. вне изображения)
    double pixel(int x, int y,
                 BorderMode mode = BorderMode::BORDER_COPY) const;
    //возвращает производный интерполированный пиксель (в т.ч. вне изображения)
    double pixel(double xApp, double yApp,
                 BorderMode mode = BorderMode::BORDER_COPY) const;
    //возвращает производный интерполированный пиксель (в т.ч. вне изображения)
    //полученный при повороте исходного изображения на угол относительно центра
    double pixel(double xApp, double yApp,
                 double xCenter, double yCenter, double angleRad,
                 BorderMode mode = BorderMode::BORDER_COPY) const;
    //устанавливает пиксель
    void setPixel(int x, int y, double value);
    //возвращает используемый формат QImage
    static QImage::Format getQImageFormat() { return _qImageFormat; }
    //дружественные классы
    friend class ImagePyramid;
    friend class ImageTransformer;

private:
    //инициализирует изображение с заданными размерами
    SubImage(int width, int height, const vector<double> &content);
    //переменные состояния
    int _width;
    int _height;
    vector<double> _content;
    //используемые константы
    static const double _maxColorValue;
    static const QImage::Format _qImageFormat;
};

#endif // SUBIMAGE_H
