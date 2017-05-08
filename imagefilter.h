#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <vector>

using namespace std;

//предоставляет класс для хранения фильтра при свертках изображений
class ImageFilter
{
public:
    ImageFilter();
    ImageFilter(int xRadius, int yRadius, const vector<double> &quots);
    ~ImageFilter();

    int xRadius() const;
    int yRadius() const;
    int width() const;
    int height() const;
    double quot(int dx, int dy) const;
    void tryNormalize();
    void evenNormalize();

private:
    int _xRadius;
    int _yRadius;
    vector<double> _quots;
};

#endif // IMAGEFILTER_H
