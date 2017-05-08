#include "imagebuilder.h"

ImageBuilder::ImageBuilder()
{
    ;
}

ImageBuilder::~ImageBuilder()
{
    ;
}

SubImage ImageBuilder::buildImageWithPoi(SubImage &source,
                                         vector<PointOfInterest> &poiVector)
{
    SubImage result  = source;

    //отрисовка точек интереса
    for (PointOfInterest poi: poiVector)
    {
        double radius = _drawQuot * poi.value();
        _drawCircle(result, poi.x(), poi.y(), radius, true);
        //отрисовка угла пика
        for (int dr = 0; dr < radius; dr++)
            result.setPixel(poi.x() + dr * std::cos(poi.peekAngle()),
                            poi.y() + dr * std::sin(poi.peekAngle()), 0.0);
    }

    //возврат результата
    return result;
}

SubImage ImageBuilder::buildImageWithBlob(SubImage &source,
                                          vector<PointOfInterest> &poiVector,
                                          int radius, double initSigma)
{
    SubImage result = source;

    //отрисовка точек интереса (блобов)
    for (PointOfInterest poi: poiVector)
    {
        int x = poi.x() * poi.coordQuot();
        int y = poi.y() * poi.coordQuot();
        double blobRadius = 1.41 * (double)radius * poi.scale() / initSigma;
        double angle = poi.peekAngle();
        _drawCircle(result, x, y, blobRadius, false);
        //отрисовка угла пика
        for (int dr = 0; dr < blobRadius; dr++)
            result.setPixel(x + dr * std::cos(angle),
                            y + dr * std::sin(angle), 1.0);
    }
    //возврат результата
    return result;
}

SubImage ImageBuilder::buildTwinImage(SubImage &left, SubImage &right,
                                      PoiCorresponder &matcher, bool fill,
                                      int radius, double initSigma)
{
    SubImage twinImage = buildTwinImage(left, right);
    int dxLeft = 0;
    int dyLeft = (twinImage.height() - left.height()) / 2;
    int dxRight = left.width();
    int dyRight = (twinImage.height() - right.height()) / 2;
    vector<pair<int, int>> corresponds = matcher.corresponds();

    for (pair<int, int> correspond: corresponds)
    {
        //отрисовка линии соответствия
        int xFirst = matcher.firstPoi(correspond.first).x() *
                     matcher.firstPoi(correspond.first).coordQuot() + dxLeft;
        int yFirst = matcher.firstPoi(correspond.first).y() *
                     matcher.firstPoi(correspond.first).coordQuot() + dyLeft;
        int xSecond = matcher.secondPoi(correspond.second).x() *
                    matcher.secondPoi(correspond.second).coordQuot() + dxRight;
        int ySecond = matcher.secondPoi(correspond.second).y() *
                    matcher.secondPoi(correspond.second).coordQuot() + dyRight;
        _drawLine(twinImage, xFirst, yFirst, xSecond, ySecond);
    }

    //отрисовка точек интереса
    int firstPoiCount = matcher.firstPoiCount();
    for (int index = 0; index < firstPoiCount; index++)
    {
        double coordQuot = matcher.firstPoi(index).coordQuot();
        double x = matcher.firstPoi(index).x() * coordQuot + dxLeft;
        double y = matcher.firstPoi(index).y() * coordQuot + dyLeft;
        double angle = matcher.firstPoi(index).peekAngle();
        if (fill)
        {
            double poiRadius = matcher.firstPoi(index).value() * _drawQuot;
            _drawCircle(twinImage, x, y, poiRadius, true);
            //отрисовка угла пика
            for (int dr = 0; dr < poiRadius; dr++)
                twinImage.setPixel(x + dr * std::cos(angle),
                                   y + dr * std::sin(angle), 0.0);
        }
        else
        {
            double blobRadius = 1.41 * (double)radius *
                                matcher.firstPoi(index).scale() / initSigma;
            _drawCircle(twinImage, x, y, blobRadius, false);
            //отрисовка угла пика
            for (int dr = 0; dr < blobRadius; dr++)
                twinImage.setPixel(x + dr * std::cos(angle),
                                   y + dr * std::sin(angle), 1.0);
        }
    }
    int secondPoiCount = matcher.secondPoiCount();
    for (int index = 0; index < secondPoiCount; index++)
    {
        double coordQuot = matcher.secondPoi(index).coordQuot();
        double x = matcher.secondPoi(index).x() * coordQuot + dxRight;
        double y = matcher.secondPoi(index).y() * coordQuot + dyRight;
        double angle = matcher.secondPoi(index).peekAngle();
        if (fill)
        {
            double poiRadius = matcher.secondPoi(index).value() * _drawQuot;
            _drawCircle(twinImage, x, y, poiRadius, true);
            //отрисовка угла пика
            for (int dr = 0; dr < poiRadius; dr++)
                twinImage.setPixel(x + dr * std::cos(angle),
                                   y + dr * std::sin(angle), 0.0);
        }
        else
        {
            double blobRadius = 1.41 * (double)radius *
                                matcher.secondPoi(index).scale() / initSigma;
            _drawCircle(twinImage, x, y, blobRadius, false);
            //отрисовка угла пика
            for (int dr = 0; dr < blobRadius; dr++)
                twinImage.setPixel(x + dr * std::cos(angle),
                                   y + dr * std::sin(angle), 1.0);
        }
    }
    //возврат результата
    return twinImage;
}

SubImage ImageBuilder::buildTwinImage(SubImage &left, SubImage &right)
{
    int leftWidth = left.width();
    int leftHeight = left.height();
    int rightWidth = right.width();
    int rightHeight = right.height();

    int resultWidth = leftWidth + rightWidth;
    int resultHeight = std::max(leftHeight, rightHeight);
    SubImage result(resultWidth, resultHeight);
    BorderMode unimpMode = BorderMode::BORDER_BLACK;

    //отрисовка левого изображения
    int dx = 0;
    int dy = (resultHeight - leftHeight) / 2;
    for (int y = 0; y < leftHeight; y++)
        for (int x = 0; x < leftWidth; x++)
            result.setPixel(x + dx, y + dy, left.pixel(x, y, unimpMode));

    //отрисовка правого изображения
    dx = leftWidth;
    dy = (resultHeight - rightHeight) / 2;
    for (int y = 0; y < rightHeight; y++)
        for (int x = 0; x < rightWidth; x++)
            result.setPixel(x + dx, y + dy, right.pixel(x, y, unimpMode));

    //возврат результата
    return result;
}

void ImageBuilder::_drawCircle(SubImage &image,
                               int x, int y, double radius, bool fill)
{
    int width = image.width();
    int height = image.height();
    //расчет полуразмера области отрисовки
    int d = (int)radius + 1;
    //отрисовка точки
    for (int dy = -1 * d; dy <= d; dy++)
    {
        for (int dx = -1 * d; dx <= d; dx++)\
        {
            //орисовка внешней кромки
            if (x + dx > -1 && x + dx < width &&
                y + dy > -1 && y + dy < height &&
                ((double)(dx * dx + dy * dy) <= radius * radius) &&
                (fill || (double)(dx * dx + dy * dy) >=
                            ((radius - 1.0) * (radius - 1.0))))
                    image.setPixel(x + dx, y + dy, 0.0);
            //отрисовка внутренний части
            if (x + dx > -1 && x + dx < width &&
                y + dy > -1 && y + dy < height &&
                ((double)(dx * dx + dy * dy) < (radius - 1.0) * (radius - 1.0)
                 && (fill || (double)(dx * dx + dy * dy) >=
                     ((radius - 2.0) * (radius - 2.0)))))
                    image.setPixel(x + dx, y + dy, 1.0);
        }
    }
    //отрисовка центра
    if (!fill)
    {
        for (int dx = -1; dx < 2; dx++)
            for (int dy = -1; dy <2; dy++)
                if (dx == 0 && dy == 0) image.setPixel(x + dx, y + dy, 1.0);
                else image.setPixel(x + dx, y + dy, 0.0);
    }
}

void ImageBuilder::_drawLine(SubImage &image, int xFirst, int yFirst,
                             int xSecond, int ySecond)
{
    int dx = xSecond - xFirst;
    int dy = ySecond - yFirst;
    double length = std::sqrt((double)(dx * dx) + (double)(dy * dy));
    for (double dl = 0.0; dl <= length; dl += 1.0)
    {
        //вычисление координат текущей точки
        double x = xFirst + (double)dx * dl / length;
        double y = yFirst + (double)dy * dl / length;
        int nearX = (int)(x + 0.5);
        int nearY = (int)(y + 0.5);
        image.setPixel(nearX, nearY, 1.0);
        image.setPixel(nearX, nearY + 1, 0.0);
        image.setPixel(nearX, nearY - 1, 0.0);
    }
}

const double ImageBuilder::_drawQuot = 25.0;
