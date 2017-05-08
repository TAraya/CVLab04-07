#include "imagetransformer.h"
#include "utils.h"

ImageTransformer::ImageTransformer(BorderMode borderMode)
{
    _borderMode = borderMode;
}

ImageTransformer::~ImageTransformer()
{
    ;
}

BorderMode ImageTransformer::borderMode()
{
    return _borderMode;
}

void ImageTransformer::setBorderMode(BorderMode mode)
{
    _borderMode = mode;
}

void ImageTransformer::convolution(SubImage &image, const ImageFilter &filter)
{
    //подготовка расширенного вектора
    vector<double> extContent =
           _extendVector(image._content, image._width, image._height, filter);
    //свертка расширенного вектора подмена исходного изображения
    image._content =
           _convoluateVector(extContent, filter, image._width, image._height);
}

void ImageTransformer::convolution(SubImage &image, const ImageFilter &hFilter,
                                                    const ImageFilter &vFilter)
{
    //подготовка расширенного вектора (гориз.)
    vector<double> extHContent =
           _extendVector(image._content, image._width, image._height, hFilter);
    //свертка расширенного вектора (гориз.)
    vector<double> convHContent =
           _convoluateVector(extHContent, hFilter, image._width, image._height);
    //подготовка расширенного вектора (гориз.)
    vector<double> extVContent =
            _extendVector(convHContent, image._width, image._height, vFilter);
    //свертка расширенного вектора (гориз.) подмена исходного изображения
    image._content =
          _convoluateVector(extVContent, vFilter, image._width, image._height);
}

SubImage ImageTransformer::convoluated(const SubImage &image,
                                       const ImageFilter &filter)
{
    //подготовка расширенного вектора
    vector<double> extContent =
           _extendVector(image._content, image._width, image._height, filter);
    //свертка расширенного вектора
    vector<double> convContent =
           _convoluateVector(extContent, filter, image._width, image._height);
    //возврат свернутого изображения
    return {image._width, image._height, convContent};
}

SubImage ImageTransformer::convoluated(const SubImage &image,
                                       const ImageFilter &xFilter,
                                       const ImageFilter &yFilter)
{
    //подготовка расширенного вектора (гориз.)
    vector<double> extHContent = 
           _extendVector(image._content, image._width, image._height, xFilter);
    //свертка расширенного вектора (гориз.)
    vector<double> convHContent =
           _convoluateVector(extHContent, xFilter, image._width, image._height);
    //подготовка расширенного вектора (гориз.)
    vector<double> extVContent = 
            _extendVector(convHContent, image._width, image._height, yFilter);
    //свертка расширенного вектора (гориз.)
    vector<double> convVContent = _convoluateVector(extVContent, yFilter,
                                                  image._width, image._height);
    //возврат свернутого изображения
    return {image._width, image._height, convVContent};
}

SubImage ImageTransformer::gaussSmoothed(const SubImage &image, double sigma)
{
    //подготовка фильтров
    ImageFilter gaussXF = Utils::gaussXFilter(sigma);
    ImageFilter gaussYF = Utils::gaussYFilter(sigma);
    //свертка изображения и возврат результата
    return convoluated(image, gaussXF, gaussYF);
}

SubImage ImageTransformer::horizontalGrad(const SubImage &image)
{
    //подготовка фильтров
    ImageFilter sobelHXF = Utils::sobelHorizontalXFilter();
    ImageFilter sobelHYF = Utils::sobelHorizontalYFilter();
    //свертка изображения и возврат результата
    return convoluated(image, sobelHXF, sobelHYF);
}

SubImage ImageTransformer::verticalGrad(const SubImage &image)
{
    //подготовка фильтров
    ImageFilter sobelVXF = Utils::sobelVerticalXFilter();
    ImageFilter sobelVYF = Utils::sobelVerticalYFilter();
    //свертка изображения и возврат результата
    return convoluated(image, sobelVXF, sobelVYF);
}

SubImage ImageTransformer::resizedUp(const SubImage &image, int quot)
{
    if (quot <= 0) return image;
    int width = image.width();
    int height = image.height();
    //расширение векторов
    int scaledWidth =  quot * width - (quot - 1);
    int scaledHeight = quot * height - (quot - 1);
    int scaledSize = scaledWidth * scaledHeight;
    vector<double> scaledContent(scaledSize, 0.0);
    //заполнение расширенных векторов известными точками
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int sourceIndex = x + y * width;
            int scaledIndex = quot * x + (quot * y) * scaledWidth;
            scaledContent[scaledIndex] = image._content[sourceIndex];
        }
    }
    //линейная интерполяция по X
    for (int y = 0; y < scaledHeight; y++)
    {
        if (y % quot == 0)
        {
            for (int x = 0; x < scaledWidth; x++)
            {
                int leftDistance = x % quot;
                int rightDistance = quot - leftDistance;
                if (leftDistance != 0)
                {
                    int index = x + y * scaledWidth;
                    int leftIndex = x - leftDistance + y * scaledWidth;
                    int rightIndex = x + rightDistance + y * scaledWidth;
                    scaledContent[index] =
                        (scaledContent[leftIndex] * (quot - leftDistance) +
                         scaledContent[rightIndex] * (quot - rightDistance))
                            / quot;
                }
            }
        }
    }
    //линейная интерполяция по Y
    for (int y = 0; y < scaledHeight; y++)
    {
        int leftDistance = y % quot;
        int rightDistance = quot - leftDistance;
        if (leftDistance != 0)
        {
            for (int x = 0; x < scaledWidth; x++)
            {
                int index = x + y * scaledWidth;
                int leftIndex = x + (y - leftDistance) * scaledWidth;
                int rightIndex = x + (y + rightDistance) * scaledWidth;
                scaledContent[index] =
                    (scaledContent[leftIndex] * (quot - leftDistance) +
                     scaledContent[rightIndex] * (quot - rightDistance))
                        / quot;
            }
        }
    }
    //возврат результата
    return {scaledWidth, scaledHeight, scaledContent};
}

SubImage ImageTransformer::resizedDown(const SubImage &image, int quot)
{
    if (quot <= 0) return image;
    int scaledWidth = image._width / quot;
    int scaledHeight = image._height / quot;
    int scaledSize = scaledWidth * scaledHeight;
    vector<double> scaledContent(scaledSize);
    //заполнение вектора итогового изображения
    for (int y = 0; y < scaledHeight; y++)
    {
        for (int x = 0; x < scaledWidth; x++)
        {
            int sourceIndex = x * quot + (y * quot) * image._width;
            int scaledIndex = x + y * scaledWidth;
            scaledContent[scaledIndex] = image._content[sourceIndex];
        }
    }
    //возврат результата
    return {scaledWidth, scaledHeight, scaledContent};
}

SubImage ImageTransformer::scaled(const SubImage &image,
                                  double hQuot, double vQuot)
{
    int scaledWidth = (int)((double)image._width * hQuot);
    int scaledHeight = (int)((double)image._height * vQuot);
    int scaledSize = scaledWidth * scaledHeight;
    vector<double> scaledContent(scaledSize, 0.0);
    for (int y = 0; y < scaledHeight; y++)
    {
        for (int x = 0; x < scaledWidth; x++)
        {
            double xApp = (double)x / hQuot;
            double yApp = (double)y / vQuot;
            scaledContent[x + y * scaledWidth] =
                image.pixel(xApp, yApp, _borderMode);
        }
    }
    //возврат результата
    return {scaledWidth, scaledHeight, scaledContent};
}

SubImage ImageTransformer::rotated(const SubImage &image, double angle, bool crop)
{
    int width = image._width;
    int height = image._height;
    //расчет новых размеров изображения
    double pi = 3.14;
    double rotateAngle = pi * angle / 180.0;
    double imageAngle = std::atan2(height, width);
    double diameter = std::sqrt((double)(width * width + height * height));
    double sin = std::min(std::sin(imageAngle - rotateAngle),
                          std::sin(imageAngle + rotateAngle));
    double cos = std::min(std::cos(imageAngle - rotateAngle),
                          std::cos(imageAngle + rotateAngle));
    int maxWidth = (int)std::abs(diameter * cos);
    int maxHeight = (int)std::abs(diameter * sin);
    //определение ограничивающей стороны
    double heightQuot = (double)maxHeight /(double)height ;
    double widthQuot = (double)maxWidth / (double)width;
    double quot = std::min(heightQuot, widthQuot);
    int subWidth = (int)(width * quot);
    int subHeight = (int)(height * quot);
    if (!crop)
    {
        subWidth = width;
        subHeight = height;
    }
    vector<double> subContent(subWidth * subHeight);

    //заполнение изображения
    double xSourceCenter = (double)width / 2.0;
    double ySourceCenter = (double)height / 2.0;
    for (int y = 0; y < subHeight; y++)
    {
        for (int x = 0; x < subWidth; x++)
        {
            subContent[x + y * subWidth] =
                    image.pixel((double)x, (double)y, xSourceCenter, ySourceCenter, rotateAngle, BorderMode::BORDER_BLACK);
        }
    }

    //возврат результата
    return {subWidth, subHeight, subContent};
}

SubImage ImageTransformer::noised(const SubImage &image, double level)
{
    vector<double> noisedContent = image._content;
    for (double &value: noisedContent)
    {
        double noise = (double)(std::rand() % 2000) / 1000.0 - 1.0;
        value += level * noise;
        if (value > 1.0) value = 1.0;
        if (value < 0.0) value = 0.0;
    }
    return {image.width(), image.height(), noisedContent};
}

SubImage ImageTransformer::brighted(const SubImage &image,
                                    double quot, double bias)
{
    vector<double> brightedContent = image._content;
    for (double &value: brightedContent)
        value = quot * value + bias;
    return {image.width(), image.height(), brightedContent};
}

SubImage ImageTransformer::biased(const SubImage &image, double angle,
                                  bool crop)
{
    double pi = 3.14;
    int width = image.width();
    int height = image.height();
    double imageAngle = std::atan2(height, width);
    double biasAngle = pi * angle / 180.0;
    
    //новая ширина и высота - из геометрических соображений
    double d = (double)width * std::sin(pi / 2.0 - biasAngle)/
                std::sin(pi / 2.0 + biasAngle - imageAngle);
    int subWidth = d * std::cos(imageAngle);
    int subHeight = d * std::sin(imageAngle);
    if (!crop)
    {
        subWidth = width;
        subHeight = height;
    }
    vector<double> subContent(subWidth * subHeight);
    double subBiasX = (double)(width - subWidth) / 2.0;
    double subBiasY = (double)(height - subHeight) / 2.0;
    
    double sin = std::sin(biasAngle);
    double dxMid = sin * (double)height / 2.0;
    for (int y = 0; y < subHeight; y++)
    {
        for (int x = 0; x < subWidth; x++)
        {
            double dx = sin * (double)(height - y - subBiasY);
            subContent[x + y * subWidth] =
                    image.pixel((double)x + dxMid - dx + subBiasX,
                               (double)y + subBiasY, BorderMode::BORDER_BLACK);
        }
    }
    
    //возврат результата
    return {subWidth, subHeight, subContent};
}


void ImageTransformer::normalize(SubImage &image)
{
    double epsilon = 0.000000001;
    if (image._content.size() < 1) return ;
    double minValue = image._content[0];
    double maxValue = image._content[0];
    for (double value: image._content)
    {
        if (minValue > value) minValue = value;
        if (maxValue < value) maxValue = value;
    }
    if ((maxValue - minValue) < epsilon) maxValue += 1.0;
    for(double &value: image._content)
        value = (value - minValue) / (maxValue - minValue);
}

void ImageTransformer::treshold(SubImage &image, double treshold)
{
    for(double &value: image._content)
        if (value < treshold) value = 0;
}

vector<double> ImageTransformer::_extendVector(const vector<double> &source,
                                               int sourceWidth,
                                               int sourceHeight,
                                               const ImageFilter &filter)
{
    int xRadius = filter.xRadius();
    int yRadius = filter.yRadius();
    int destWidth = sourceWidth + 2 * xRadius;
    int destHeight = sourceHeight + 2 * yRadius;
    int destSize = destWidth * destHeight;
    vector<double> result(destSize);
    //построение расширенного вектора
    for(int index = 0; index < destSize; index++)
    {
        int x = index % destWidth;
        int y = index / destWidth;
        if (x >= xRadius && x < xRadius + sourceWidth &&
            y >= yRadius && y < yRadius + sourceHeight)
        {
            //копируем значение из исходного вектора
            int sourceX = x - xRadius;
            int sourceY = y - yRadius;
            result[index] = source[sourceX + sourceY * sourceWidth];
        }
        else
        {
            //дополняем вектор в зависимости от метода расширения
            switch (_borderMode)
            {
            case BorderMode::BORDER_BLACK:
            {
                result[index] = 0.0;
                break;
                //окончание блока BORDER_BLACK
            }
            case BorderMode::BORDER_COPY:
            {
                //расчет координат ближайщей границы
                int xBorder = x - xRadius;
                if (xBorder < 0) xBorder = 0;
                if (x >= xRadius + sourceWidth) xBorder = sourceWidth - 1;
                int yBorder = y - yRadius;
                if (yBorder < 0) yBorder = 0;
                if (y >= yRadius + sourceHeight) yBorder = sourceHeight - 1;
                int borderIndex = xBorder + yBorder * sourceWidth;
                result[index] = source[borderIndex];
                break;
                //окончание блока BORDER_COPY
            }
            case BorderMode::BORDER_MIRROR:
            {
                //расчет координат отраженной точки
                int xMirror = x - xRadius;
                int dX = 0;
                if (x < xRadius)
                {
                    dX = xRadius - x - 1;
                    xMirror = dX;
                }
                if (x >= xRadius + sourceWidth)
                {
                    dX = x - xRadius - sourceWidth;
                    xMirror = sourceWidth - dX - 1;
                }
                if (xMirror >= sourceWidth) xMirror = sourceWidth - 1;
                if (xMirror < 0) xMirror = 0;
                int yMirror = y - yRadius;
                int dY = 0;
                if (y < yRadius)
                {
                    dY = yRadius - y - 1;
                    yMirror = dY;
                }
                if (y >= yRadius + sourceHeight)
                {
                    dY = y - yRadius - sourceHeight;
                    yMirror = sourceHeight - dY - 1;
                }
                if (yMirror >= sourceHeight) yMirror = sourceHeight - 1;
                if (yMirror < 0) yMirror = 0;
                int mirrorIndex = xMirror + yMirror * sourceWidth;
                result[index] = source[mirrorIndex];
                break;
                //окончание блока BORDER_MIRROR
            }
            case BorderMode::BORDER_CYCLE:
            {
                //расчет координат завернутой точки
                int xCycle = x - xRadius;
                while (xCycle < -1) xCycle += sourceWidth;
                xCycle %= sourceWidth;
                int yCycle = y - yRadius;
                while (yCycle < -1) yCycle += sourceHeight;
                yCycle %= sourceHeight;
                int cycleIndex = xCycle + yCycle * sourceWidth;
                result[index] = source[cycleIndex];
                break;
                //окончание блока BORDER_CYCLE
            }
            //окончание блока switch
            }
        }
        //окончание блока for
    }
    //окончание метода extendVector
    return result;
}

vector<double> ImageTransformer::_convoluateVector(const vector<double> &extSource,
                                         const ImageFilter &filter,
                                         int destWidth, int destHeight)
{
    int xRadius = filter.xRadius();
    int yRadius = filter.yRadius();
    int extSrcWidth = destWidth + 2 * xRadius;
    int destSize = destWidth * destHeight;
    vector<double> result(destSize);
    for (int y = yRadius; y < yRadius + destHeight; y++)
    {
        for (int x = xRadius; x < xRadius + destWidth; x++)
        {
            double value = 0.0;
            for (int dY = -1 * yRadius; dY <= yRadius; dY++)
            {
                for (int dX = -1 * xRadius; dX <= xRadius; dX++)
                {
                    int subIndex = x + dX + (y + dY) * extSrcWidth;
                    value += extSource[subIndex] * filter.quot(dX, dY);
                }
            }
            int destX = x - xRadius;
            int destY = y - yRadius;
            result[destX + destY * destWidth] = value;
        }
    }
    return result;
}

