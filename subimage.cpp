#include "subimage.h"

SubImage::SubImage()
{
    ;
}

SubImage::SubImage(const QImage &image)
{
    loadImage(image);
}

SubImage::SubImage(int width, int height)
{
    _width = width;
    _height = height;
    _content.resize(_width * _height);
    for (double &value: _content) value = 0.0;
}

SubImage::~SubImage()
{
    ;
}

SubImage::SubImage(int width, int height, const vector<double> &content)
{
    _width = width;
    _height = height;
    _content = content;
}

void SubImage::loadImage(const QImage &image)
{
    _width = image.width();
    _height = image.height();
    int size = _width * _height;
    _content.resize(size);
    //поточечная запись компонент в вектора
    //хранение в векторах построчное
    int index = 0;
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            QRgb pixel = image.pixel(x, y);
            _content[index] = qGray(pixel) / _maxColorValue;
            index++;
        }
    }
}

QImage SubImage::getQImage() const
{
    QImage result(_width, _height, _qImageFormat);
    //функция корректировки компоненты в диапазон 0.0 .. 1.0
    auto trim = [](double value)
    {
        if (value < 0.0) return 0.0;
        if (value > 1.0) return 1.0;
        return value;
    };
    //поточечное восстановление изображения
    int index = 0;
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            result.setPixel(x, y,
                    qRgb((int) (trim(_content[index]) * _maxColorValue),
                         (int) (trim(_content[index]) * _maxColorValue),
                         (int) (trim(_content[index]) * _maxColorValue)));
            index++;
        }
    }
    return result;
}

int SubImage::width() const
{
    return _width;
}

int SubImage::height() const
{
    return _height;
}

double SubImage::pixel(double xApp, double yApp, BorderMode mode) const
{
    //определение 4 точек в исходном изображении, ближайших к
    // аппроксимируемой точке
    int xLeft = (int)std::floor(xApp);
    int xRight = xLeft + 1;
    int yUp = (int)std::floor(yApp);
    int yDown = yUp + 1;
    //если точки выходят за границы изображения - аппроксимация согласно
    // выбранному режиму обработки краев
    double leftDistance = xApp - (double)xLeft;
    double rightDistance = (double)xRight - xApp;
    double upDistance = yApp - (double)yUp;
    double downDistance = yDown - (double)yApp;
    //расчет промежуточных интерполируемых значений по вертикалям
    double leftMidValue = upDistance * pixel(xLeft, yDown, mode)
                            + downDistance * pixel(xLeft, yUp, mode);
    double rightMidValue = upDistance * pixel(xRight, yDown, mode)
                            + downDistance * pixel(xRight, yUp, mode);
    //линейная интерполяция ожидаемого значения
    return leftDistance * rightMidValue + rightDistance * leftMidValue;
}

double SubImage::pixel(double xApp, double yApp,
                       double xCenter, double yCenter, double angleRad,
                       BorderMode mode) const
{
    double dx = xApp - xCenter;
    double dy = yApp - yCenter;
    double radius = std::sqrt(dx * dx + dy * dy);
    double initAngle = std::atan2(dy, dx);
    //расчет новых координат
    double x = xCenter + radius * std::cos(initAngle - angleRad);
    double y = yCenter + radius * std::sin(initAngle - angleRad);
    //возврат результата
    return pixel(x, y, mode);
}

double SubImage::pixel(int x, int y, BorderMode mode) const
{
    if (x > -1 && x < _width && y > -1 && y < _height)
        return _content[x + y * _width];
    //если x или y находятся вне исходного изображения
    switch (mode)
    {
    case BorderMode::BORDER_BLACK:
    {
        return 0.0;
        break;
        //окончание блока BORDER_BLACK
    }
    case BorderMode::BORDER_COPY:
    {
        //расчет координат ближайщей границы
        int xBorder = x;
        if (xBorder < 0) xBorder = 0;
        if (x >= _width) xBorder = _width - 1;
        int yBorder = y;
        if (yBorder < 0) yBorder = 0;
        if (y >= _height) yBorder = _height - 1;
        return _content[xBorder + yBorder * _width];
        break;
        //окончание блока BORDER_COPY
    }
    case BorderMode::BORDER_MIRROR:
    {
        //расчет координат отраженной точки
        int xMirror = x;
        int dX = 0;
        if (x < 0)
        {
            dX = -x - 1;
            xMirror = dX;
        }
        if (x >= _width)
        {
            dX = x - _width;
            xMirror = _width - dX - 1;
        }
        if (xMirror >= _width) xMirror = _width - 1;
        if (xMirror < 0) xMirror = 0;
        int yMirror = y;
        int dY = 0;
        if (y < 0)
        {
            dY = -y - 1;
            yMirror = dY;
        }
        if (y >= _height)
        {
            dY = y - _height;
            yMirror = _height - dY - 1;
        }
        if (yMirror >= _height) yMirror = _height - 1;
        if (yMirror < 0) yMirror = 0;
        return _content[xMirror + yMirror * _width];
        break;
        //окончание блока BORDER_MIRROR
    }
    case BorderMode::BORDER_CYCLE:
    {
        //расчет координат завернутой точки
        int xCycle = x;
        while (xCycle < 0) xCycle += _width;
        xCycle %= _width;
        int yCycle = y;
        while (yCycle < 0) yCycle += _height;
        yCycle %= _height;
        return _content[xCycle + yCycle * _width];
        break;
        //окончание блока BORDER_CYCLE
    }
    //окончание блока switch
    }
}

void SubImage::setPixel(int x, int y, double value)
{
    if (x > -1 && x < _width &&
        y > -1 && y < _height)
        _content[x + y * _width] = value;
}

const double SubImage::_maxColorValue = 255.0;
const QImage::Format SubImage::_qImageFormat = QImage::Format_Grayscale8;
