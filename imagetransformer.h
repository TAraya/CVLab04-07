#ifndef IMAGETRANSFORMER_H
#define IMAGETRANSFORMER_H

#include "enumerations.h"
#include "imagefilter.h"
#include "subimage.h"

//предоставляет методы для преобразования изображений,
//хранит некоторые настройки преобразований
class ImageTransformer
{
public:
    ImageTransformer(BorderMode borderMode);
    ~ImageTransformer();
    //параметр обработки краев
    BorderMode borderMode();
    void setBorderMode(BorderMode mode);
    //методы для свертки изображении
    void convolution(SubImage &image, const ImageFilter &filter);
    void convolution(SubImage &image, const ImageFilter &xFilter,
                                      const ImageFilter &yFilter);
    SubImage convoluated(const SubImage &image, const ImageFilter &filter);
    SubImage convoluated(const SubImage &image, const ImageFilter &xFilter,
                                                const ImageFilter &yFilter);
    //возврашает изображение, сглаженное фильтром Гаусса
    SubImage gaussSmoothed(const SubImage &image, double sigma);
    //возврашиет карту горизонтальных градиентов
    SubImage horizontalGrad(const SubImage &image);
    //возвращает карту вертикальных градиентов
    SubImage verticalGrad(const SubImage &image);
    //возвращает изображение, кратно увеличенное в целочисленный quot
    SubImage resizedUp(const SubImage &image, int quot);
    //возвращает изображение, кратно уменьшенное в целочисленный quot
    SubImage resizedDown(const SubImage &image, int quot);
    //возвращает масштабированное изображение в рац. hQuot, vQuot
    SubImage scaled(const SubImage &image, double hQuot, double vQuot);
    //возвращает изображение, повернутое на angle град.
    SubImage rotated(const SubImage &image, double angle, bool crop);
    //возвращает изображение с добавленными шумами в диапазоне +-level
    SubImage noised(const SubImage &image, double level);
    //возвращает изображение с преобразованной яркостью *quot + bias
    SubImage brighted(const SubImage &image, double quot, double bias);
    //возвращает сдвинутое на угол angle град. изображение
    SubImage biased(const SubImage &image, double angle, bool crop);
    //нормализует изображение в диапазон 0.0 .. 1.0
    void normalize(SubImage &image);
    //пороговая фильтрация
    void treshold(SubImage &image, double treshold);

private:
    BorderMode _borderMode;
    //возвращает расширенный на размер фильтра вектор для последующей свертки
    vector<double> _extendVector(const vector<double> &source,
                                 int sourceWidth, int sourceHeight,
                                 const ImageFilter &filter);
    //возвращает свернутый вектор на основе расширенного вектора
    vector<double> _convoluateVector(const vector<double> &extSource,
                                     const ImageFilter &filter,
                                     int resultWidth, int resultHeight);
};

#endif // IMAGETRANSFORMER_H
