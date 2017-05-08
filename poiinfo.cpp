#include "poiinfo.h"
#include "imagetransformer.h"
#include "imagebuilder.h"
#include "utils.h"

PoiInfo::PoiInfo()
{
    _isInit = false;
    _isDescriptorsCalc = false;
    _isHarrysParametersSetted = false;
    _isDescriptorParametersSetted = false;
    _isOrientationParametersSetted = false;
    _poiCount = -1;
}

PoiInfo::~PoiInfo()
{
    ;
}

bool PoiInfo::isInit() const
{
    return _isInit;
}

bool PoiInfo::isDescriptorsCalc() const
{
    return _isDescriptorsCalc;
}

bool PoiInfo::isHarrysParametersSetted() const
{
    return _isHarrysParametersSetted;
}

bool PoiInfo::isDescriptorParametersSetted() const
{
    return _isDescriptorParametersSetted;
}

bool PoiInfo::isOrientationParametersSetted() const
{
    return _isOrientationParametersSetted;
}

void PoiInfo::setHarrysParameters(int radius, double sigma, int localMaxRadius,
                                  double treshold, double maskSigma,
                                  BorderMode mode)
{
    _harrysRadius = radius;
    _harrysSigma = sigma;
    _harrysLocalMaxRadius = localMaxRadius;
    _harrysTreshold = treshold;
    _harrysMaskSigma = maskSigma;
    _borderMode = mode;
    _isInit = false;
    _isHarrysParametersSetted = true;
}

void PoiInfo::setDescriptorParameters(int descRadius, int descNetSize,
                                      double descMaskSigma, int descHystSize)
{
    _descRadius = descRadius;
    _descNetSize = descNetSize;
    _descMaskSigma = descMaskSigma;
    _descHystSize = descHystSize;
    _isDescriptorsCalc = false;
    _isDescriptorParametersSetted = true;
}

void PoiInfo::setOrientationParameters(int orientRadius,
                                       double orientMaskSigma,
                                       int orientHystSize)
{
    _orientRadius = orientRadius;
    _orientMaskSigma = orientMaskSigma;
    _orientHystSize = orientHystSize;
    _isDescriptorsCalc = false;
    _isOrientationParametersSetted = true;
}

void PoiInfo::initHarrysDetector(const SubImage &image)
{
    _isInit = false;
    //возврат если параметры детектора не установлены
    if (!_isHarrysParametersSetted) return ;

    int width = image.width();
    int height = image.height();
    ImageTransformer transformer(_borderMode);

    //сглаживание фильтром Гаусса
    SubImage source = transformer.gaussSmoothed(image, _harrysSigma);

    //расчет проекций градиента
    SubImage xGrad = transformer.horizontalGrad(source);
    SubImage yGrad = transformer.verticalGrad(source);

    //расчет вспомогательных представлений IxIx, IxIy, IyIy
    SubImage iXXview(width, height);
    SubImage iYYview(width, height);
    SubImage iXYview(width, height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double dx = xGrad.pixel(x, y, _borderMode);
            double dy = yGrad.pixel(x, y, _borderMode);
            iXXview.setPixel(x, y, dx * dx);
            iXYview.setPixel(x, y, dx * dy);
            iYYview.setPixel(x, y, dy * dy);
        }
    }
    //расчет коэфф. A,B,C матрицы H - аналогично свертке
    //представлений IxIx, IxIy, IyIy с маской ()
    ImageFilter maskX = Utils::gaussXFilter(_harrysMaskSigma, _harrysRadius);
    ImageFilter maskY = Utils::gaussYFilter(_harrysMaskSigma, _harrysRadius);
    transformer.convolution(iXXview, maskX, maskY);
    transformer.convolution(iXYview, maskX, maskY);
    transformer.convolution(iYYview, maskX, maskY);

    //построение карты точек интереса
    SubImage points(width, height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            points.setPixel(x, y,
                _getHarrysError(iXXview.pixel(x, y, _borderMode),
                                iXYview.pixel(x, y, _borderMode),
                                iYYview.pixel(x, y, _borderMode)));

    //пороговая фильтрация и определение локальных максимумов
    transformer.treshold(points, _harrysTreshold);
    _localMaxOnly(points, _harrysLocalMaxRadius, _borderMode);

    //запись точек интереса
    _poiVector.clear();
    _poiCount = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double value = points.pixel(x, y, _borderMode);
            if  (value >= _harrysTreshold)
            {
                PointOfInterest poi(x, y, value);
                _poiVector.push_back(poi);
                _poiCount++;
            }
        }
    }
    _diameter = std::sqrt(width * width + height * height);
    _isInit = true;
}

void PoiInfo::initSiftDetector(const ImagePyramid &pyramid)
{
    _isInit = false;
    _poiVector.clear();
    _poiCount = 0;

    //вспомогательный объект для подсчета функции Харриса
    PoiInfo utilPInfo;
    utilPInfo.setHarrysParameters(_harrysRadius, _harrysSigma, 1, 0.0,
                                    _harrysMaskSigma, _borderMode);

    //поиск локальных экстремумов в пирамиде DoG
    int octaves = pyramid.octaves();
    int levels = pyramid.levels();
    for (int octave = 0; octave < octaves; octave++)
    {
        for (int level = 1; level < levels; level++)
        {
            int width = pyramid._dogPyramid[octave][level].width();
            int height = pyramid._dogPyramid[octave][level].height();
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    bool isPoi = pyramid._isDogExtremum(x, y, octave, level);
                    if (!isPoi) continue;

                    //уточнение координат (только по x, y)
                    int xCurr = x, yCurr = y;
                    double xBias = 0.0, yBias = 0.0;

                    //значения производных в точке x, y
                    double derX1 = 0.0, derY1 = 0.0;
                    double derXX = 0.0, derYY = 0.0, derXY = 0.0;
                    bool biasFlag = false, outFlag = false;
                    int step = 0; int maxStep = 100;
                    do
                    {
                        biasFlag = false;
                        outFlag = false;
                        //вспомогательная матрица 5 * 5 для расчета производных
                        vector<double> values(25);
                        for (int dy = -2; dy <= 2; dy++)
                            for (int dx = -2; dx <= 2; dx++)
                                values[2 + dx + (2 + dy) * 5] =
                                    pyramid._dogPyramid[octave][level].pixel(
                                                       xCurr + dx, yCurr + dy);

                        //расчет значений производных (по x и y)
                        vector<double> derX(9);
                        for (int dy = -1; dy <= 1; dy++)
                            for (int dx = -1; dx <= 1; dx++)
                                derX[1 + dx + (1 + dy) * 3] = 0.5 *
                                    (values[2 + dx + 1 + (2 + dy) * 5] -
                                        values[2 + dx - 1 + (2 + dy) * 5]);

                        vector<double> derY(9);
                        for (int dy = -1; dy <= 1; dy++)
                            for (int dx = -1; dx <= 1; dx++)
                                derY[1 + dx + (1 + dy) * 3] = 0.5 *
                                    (values[2 + dx + (2 + dy + 1) * 5] -
                                        values[2 + dx + (2 + dy - 1) * 5]);

                        //первые производные в точке x, y
                        derX1 = derX[1 + 0 + (1 + 0) * 3];
                        derY1 = derY[1 + 0 + (1 + 0) * 3];
                        //вторые производные в точке x, y
                        derXX = 0.5 * (derX[1 + 1 + (1 + 0) * 3] -
                                            derX[1 - 1 + (1 + 0) * 3]);
                        derYY = 0.5 * (derY[1 + 0 + (1 + 0 + 1) * 3] -
                                            derY[1 + 0 + (1 + 0 - 1) * 3]);
                        derXY = 0.5 * (derX[1 + 0 + (1 + 1) * 3] -
                                            derX[1 + 0 + (1 - 1) * 3]);

                        // для отладочных целей должно равняться derXY - верно
                        // double derYX = 0.5 * (derY[1 + 0 + 1 + (1 + 0) * 3] -
                        //                    derY[1 + 0 - 1 + (1 + 0) * 3]);

                        if (_abs(derXX) > _epsilon) xBias = -1.0 * derX1 / derXX;
                        else xBias = 0.0;
                        if (_abs(derYY) > _epsilon) yBias = -1.0 * derY1 / derYY;
                        else yBias = 0.0;
                        //ищем наибольшее смещение, если смещение более 0,5
                        // - смещаем точку и пересчитываем
                        if (_abs(xBias) >= _abs(yBias))
                        {
                            if (xBias > 0.5)
                            {
                                xCurr++;
                                biasFlag = true;
                            }
                            if (xBias < -0.5)
                            {
                                xCurr--;
                                biasFlag = true;
                            }
                        }
                        else
                        {
                            if (yBias > 0.5)
                            {
                                yCurr++;
                                biasFlag = true;
                            }
                            if (yBias < -0.5)
                            {
                                yCurr--;
                                biasFlag = true;
                            }
                        }
                        step++;
                        //проверка выхода за границы
                        if (x < 0 || x >= width || y < 0 || y >= height)
                            outFlag = true;
                    } while ((step < maxStep) && biasFlag && !outFlag);

                    //при выходе за границы - отброс точки
                    if (xCurr + xBias < 0.0 || xCurr + xBias >= width ||
                        yCurr + yBias < 0.0 || yCurr + yBias >= height)
                            continue;

                    //удаление низкоконтрастных точек
                    double value =
                        pyramid._dogPyramid[octave][level].pixel(xCurr, yCurr)
                            + 0.5 * derX1 * xBias + 0.5 * derY1 * yBias;

                    if (_abs(value) < 0.03) continue;

                    //отброс краевых точек
                    double trH = derXX + derYY;
                    double detH = derXX * derYY - derXY * derXY;
                    if (trH * trH / detH > 12.1) continue; //при r = 10

                    double sigma = pyramid._sigmas[octave][level];
                    //сопоставление с функцией Харриса (нормализуем к масштабу)
                    if (utilPInfo._harrysValue(
                                pyramid._pyramid[octave][level + 1],
                                xCurr, yCurr) >
                            _blobHarrysLimit / (sigma * sigma)) continue;

                    //расчет коэфф. масштабирования для координат
                    double coordQuot = pyramid._sigmas[octave][0] /
                                                            pyramid._initSigma;

                    //запись точки интереса
                    PointOfInterest poi(xCurr + xBias, yCurr + yBias, 1.0,
                                        0.0, sigma, coordQuot);
                    _poiVector.push_back(poi);
                    _poiCount++;
                }
            }
        }
    }
    _isInit = true;
}

void PoiInfo::calcPeekAngles(const SubImage &image)
{
    if (!_isOrientationParametersSetted) return ;

    //вспомогательный вектор
    vector<PointOfInterest> secondPeeksPoiVector(0);

    for(PointOfInterest &poi: _poiVector)
    {
        //определение пиковых направлений
        bool hasSecondPeek = false;
        double secondPeekAngle = 0.0;
        double peekAngle =
            poi.calcPeekAngle(image, _orientHystSize,  _orientRadius,
                             _orientMaskSigma, hasSecondPeek, secondPeekAngle);
        poi.setPeekAngle(peekAngle);
        //если имеется второе пиковое направление - добавляем ещё одну точку
        if (hasSecondPeek)
        {
            PointOfInterest secondPoi(poi.x(), poi.y(), poi.value(),
                                      secondPeekAngle, poi.scale(),
                                      poi.coordQuot());
            secondPeeksPoiVector.push_back(secondPoi);
        }
    }
    //запись добавленных точек
    for(PointOfInterest poi: secondPeeksPoiVector)
        _poiVector.push_back(poi);
}

void PoiInfo::calcPeekAngles(const ImagePyramid &pyramid)
{
    if (!_isOrientationParametersSetted) return ;

    //вспомогательный вектор
    vector<PointOfInterest> secondPeeksPoiVector(0);

    for(PointOfInterest &poi: _poiVector)
    {
        //поиск ближайщего по масштабу изображения
        int octave = 0, level = 0;
        pyramid.nearestImageIndexes(poi.scale(), octave, level);

        //расчет размера окна, на котором будет определяться пиковый угол
        double scaleQuot = pyramid._sigmas[octave][level] /
                                                    pyramid._sigmas[octave][0];
        int currRadius = (int)((double)_orientRadius * scaleQuot);

        //определение пиковых направлений
        bool hasSecondPeek = false;
        double secondPeekAngle = 0.0;
        double peekAngle =
            poi.calcPeekAngle(pyramid._pyramid[octave][level],
                              _orientHystSize, currRadius, _orientMaskSigma,
                              hasSecondPeek, secondPeekAngle);
        poi.setPeekAngle(peekAngle);

        //если имеется второе пиковое направление - добавляем ещё одну точку
        if (hasSecondPeek)
        {
            PointOfInterest secondPoi(poi.x(), poi.y(), poi.value(),
                                      secondPeekAngle, poi.scale(),
                                      poi.coordQuot());
            secondPeeksPoiVector.push_back(secondPoi);
        }
    }
    //запись добавленных точек
    for(PointOfInterest poi: secondPeeksPoiVector)
        _poiVector.push_back(poi);
}

void PoiInfo::calcDescriptors(const SubImage &image,
                              bool consRotate, NormalizeMode mode)
{
    if (!_isDescriptorParametersSetted) return ;

    for(PointOfInterest &poi: _poiVector)
    {
        //расчет дескрипторов
        poi.initDesc(_descNetSize, _descHystSize);
        poi.calcDesc(image, _descRadius, _descMaskSigma, consRotate);

        //нормализация
        if (mode == NormalizeMode::SINGLE_NORMALIZE)
                poi.normalizeDescSingle();
        if (mode == NormalizeMode::COMPLEX_NORMALIZE)
                poi.normalizeDescComplex();
        if (mode == NormalizeMode::COMPLEX_TRIM_NORMALIZE)
                poi.normalizeDescTrimComplex();
    }

    _isDescriptorsCalc = true;
}

void PoiInfo::calcDescriptors(const ImagePyramid &pyramid,
                              bool consRotate, NormalizeMode mode)
{
    if (!_isDescriptorParametersSetted) return ;

    for(PointOfInterest &poi: _poiVector)
    {
        //поиск ближайщего по масштабу изображения
        int octave = 0, level = 0;
        pyramid.nearestImageIndexes(poi.scale(), octave, level);

        //расчет размера окна дескриптора
        double scaleQuot = pyramid._sigmas[octave][level] /
                                                    pyramid._sigmas[octave][0];
        int currRadius = (int)((double)_descRadius * scaleQuot);

        //расчет дескрипторов
        poi.initDesc(_descNetSize, _descHystSize);
        poi.calcDesc(pyramid._pyramid[octave][level], currRadius,
                     _descMaskSigma, consRotate);

        //нормализация
        if (mode == NormalizeMode::SINGLE_NORMALIZE)
                poi.normalizeDescSingle();
        if (mode == NormalizeMode::COMPLEX_NORMALIZE)
                poi.normalizeDescComplex();
        if (mode == NormalizeMode::COMPLEX_TRIM_NORMALIZE)
                poi.normalizeDescTrimComplex();
    }

    _isDescriptorsCalc = true;
}

int PoiInfo::poiCount() const
{
    return _poiCount;
}

vector<PointOfInterest> PoiInfo::poiVector(int poiMaxCount, int &poiCount)
const
{
    //если объект не инициализирован
    if (!_isInit)
    {
        poiCount = -1;
        return vector<PointOfInterest>(0);
    }
    //запрашиваются все точки или больше, чем имеется
    if (poiMaxCount < 1 || poiMaxCount >= _poiCount)
    {
        //возврат всех найденных точек интереса
        poiCount = _poiCount;
        return vector<PointOfInterest>(_poiVector);
    }
    else
    {
        //маска для точек, соответствующих условию
        vector<bool> mask(_poiCount, false);
        //бинарный поиск
        double maxRadius = _diameter;
        double minRadius = 0.0;
        int currCount;
        bool end = false;
        while (!end)
        {
            double midRadius = (maxRadius + minRadius) / 2.0;
            currCount = _localMaxPoiCoutAtRadius(midRadius, mask);
            if (currCount < poiMaxCount)
            {
                //уменьшаем радиус локального максимума
                maxRadius = midRadius;
            }
            if (currCount > poiMaxCount)
            {
                //увеличиваем радиус локального максимума
                minRadius = midRadius;
            }
            //условие завершения поиска
            end = (currCount == poiMaxCount) ||
                  ((maxRadius - minRadius) < 0.5);
        }
        //подготовка результата
        poiCount = currCount;
        vector<PointOfInterest> result(currCount);
        int count = 0;
        for (int index = 0; index < _poiCount; index++)
        {
            if (mask[index])
            {
                result[count] = _poiVector[index];
                count++;
            }
        }
        //возврат результата
        return result;
    }
}

void PoiInfo::resetIsInit()
{
    _poiVector.clear();
    _poiCount = -1;
    _isInit = false;
}

double PoiInfo::_harrysValue(const SubImage &image, int x, int y)
{
    //возврат если параметры детектора не установлены
    if (!_isHarrysParametersSetted) return 0.0;

    //построение вспомогательного представления
    int width = _harrysRadius;
    int height = _harrysRadius;
    SubImage subSource(width, height);
    for (int dy = -1 * _harrysRadius; dy <= _harrysRadius; dy++)
        for (int dx = -1 * _harrysRadius; dx <= _harrysRadius; dx++)
            subSource.setPixel(_harrysRadius + dx, _harrysRadius + dy,
                               image.pixel(x + dx, y + dy));
    ImageTransformer transformer(_borderMode);
    //сглаживание фильтром Гаусса
    SubImage source = transformer.gaussSmoothed(subSource, _harrysSigma);

    //расчет проекций градиента
    SubImage xGrad = transformer.horizontalGrad(source);
    SubImage yGrad = transformer.verticalGrad(source);

    //расчет вспомогательных представлений IxIx, IxIy, IyIy
    SubImage iXXview(width, height);
    SubImage iYYview(width, height);
    SubImage iXYview(width, height);
    for (int sy = 0; sy < height; sy++)
    {
        for (int sx = 0; sx < width; sx++)
        {
            double dx = xGrad.pixel(sx, sy, _borderMode);
            double dy = yGrad.pixel(sx, sy, _borderMode);
            iXXview.setPixel(sx, sy, dx * dx);
            iXYview.setPixel(sx, sy, dx * dy);
            iYYview.setPixel(sx, sy, dy * dy);
        }
    }
    //расчет коэфф. A,B,C матрицы H - аналогично свертке
    //представлений IxIx, IxIy, IyIy с маской ()
    ImageFilter maskX = Utils::gaussXFilter(_harrysMaskSigma, _harrysRadius);
    ImageFilter maskY = Utils::gaussYFilter(_harrysMaskSigma, _harrysRadius);
    transformer.convolution(iXXview, maskX, maskY);
    transformer.convolution(iXYview, maskX, maskY);
    transformer.convolution(iYYview, maskX, maskY);

    //возврат результата
    return _getHarrysError(
                iXXview.pixel(_harrysRadius, _harrysRadius, _borderMode),
                iXYview.pixel(_harrysRadius, _harrysRadius, _borderMode),
                iYYview.pixel(_harrysRadius, _harrysRadius, _borderMode));
}

double PoiInfo::_getHarrysError(double a, double b, double c)
{
    /* ищем собственные числа для матрицы |a b|
     *                                    |b c|
     * det |a-h b  |
     *     |b   c-h| = (a-h) * (с-h) - bb = aс - ah - сh + hh - bb =
     * = hh - (a + с) * h + (aс - bb) = 0;
     * D = (a + с)^2 - 4 * (aс - bb) = aa + 2ac + cc - 4ac + 4bb =
     * = aa - 2ac + cc + 4bb = (a - c)^2 + (2b)^2 >= 0
     * h1 = ((a + b) + sqrt(D)) / 2
     * h2 = ((a + b) - sqrt(D)) / 2 */
    double d = (a - c) * (a - c) + 4.0 * b * b;
    double h1 = ((a + c) + std::sqrt(d)) / 2.0;
    double h2 = ((a + c) - std::sqrt(d)) / 2.0;
    // f = Hmin
    return std::min(h1, h2);
}

double PoiInfo::_abs(double x)
{
    if (x < 0.0) return -1.0 * x;
    return x;
}

int PoiInfo::_localMaxPoiCoutAtRadius(double radius, vector<bool> &mask)
const
{
    int count = 0;
    for (int i = 0; i < _poiCount; i++)
    {
        bool flag = true;
        //сравнение i-ой точки с каждой j-ой
        for (int j = 0; j < _poiCount && flag; j++)
        {
            double dx = _poiVector[i].x() - _poiVector[j].x();
            double dy = _poiVector[i].y() - _poiVector[j].y();
            if ((dx * dx + dy * dy) < (radius * radius) && i != j &&
                    _poiVector[i].value() < _localMaxPoiQuot *
                                                _poiVector[j].value())
                flag = false;
        }
        mask[i] = flag;
        if (flag) count++;
    }
    return count;
}

void PoiInfo::_localMaxOnly(SubImage &poiImage, double radius,
                            BorderMode mode)
const
{
    int width = poiImage.width();
    int height = poiImage.height();

    //определение локальных максимумов в пределах радиуса
    vector<bool> localMaxMask(width * height, false);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            localMaxMask[x + y * width] =
                    _isLocalMax(poiImage, x, y, radius, mode);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (!localMaxMask[x + y * width])
                poiImage.setPixel(x, y, 0.0);
}

bool PoiInfo::_isLocalMax(SubImage &poiImage, int x, int y, double radius,
                              BorderMode mode) const
{
    double value = poiImage.pixel(x, y, mode);
    for (int dx = -1 * radius; dx <= radius; dx++)
    {
        for (int dy = -1 * radius; dy <= radius; dy++)
        {
            bool inRadius = ((dx * dx + dy * dy) <= radius * radius);
            bool isCenter = (dx == 0 && dy == 0);
            //если в окрестнсти находится пиксель ярче - возвращаем false
            if (inRadius && !isCenter &&
                value < _localMaxPoiQuot * poiImage.pixel(x + dx, y + dy, mode))
                    return false;
        }
    }
    //если в окрестнсти не нашлось пикселя ярче - возвращаем true
    return true;
}


