#include "imagepyramid.h"
#include "imagetransformer.h"
#include "utils.h"

ImagePyramid::ImagePyramid()
{
    _isInit = false;
}

ImagePyramid::~ImagePyramid()
{
    ;
}

void ImagePyramid::init(const SubImage &image, int levels, double initSigma, bool subZeroOctave, BorderMode mode)
{
    _subZeroOctave = subZeroOctave;
    _levels = levels;
    _initSigma = initSigma;
    _countOctaves(image._width, image._height);
    //расчет размеров структур данных
    //всего _octaves (+1) октава, в каждой _levels + 2 уровня
    _sigmas.resize(_octaves);
    _pyramid.resize(_octaves);
    _dogPyramid.resize(_octaves);
    for (int octave = 0; octave < _octaves; octave++)
    {
        _sigmas[octave].resize(_levels + 2);
        _pyramid[octave].resize(_levels + 2);
        _dogPyramid[octave].resize(_levels + 1);
    }
    _calcSigmas();
    _buildPyramid(image, mode);
    _buildDogPyramid();
    _isInit = true;
}

bool ImagePyramid::isInit() const
{
    return _isInit;
}

bool ImagePyramid::subZeroOctave() const
{
    return _subZeroOctave;
}

int ImagePyramid::levels() const
{
    return _levels;
}

int ImagePyramid::octaves() const
{
    return _octaves;
}

double ImagePyramid::initSigma() const
{
    return _initSigma;
}

void ImagePyramid::nearestImageIndexes(double sigma, int &octave, int &level)
const
{
    for (octave = 0; octave < _octaves - 1 &&
                      sigma > 2.0 * _sigmas[octave][0]; octave++) ;
    for (level = 0; level < _levels + 1 &&
             sigma >= _sigmas[octave][level] + _epsilon; level++) ;
    int y = 0;
}

void ImagePyramid::_countOctaves(int width, int height)
{
    double minDimension = 1.0;
    int byWidth = (int)std::log2((float)width / minDimension);
    int byHeight = (int)std::log2((float)height / minDimension);
    _octaves = std::min(byWidth, byHeight);
    if (_subZeroOctave) _octaves++;
}

void ImagePyramid::_calcSigmas()
{
    //коэффициент для следующего уровня
    double quot = std::pow(2.0, 1.0 / (double)(_levels - 1));
    //учет возможной -1 октавы
    if (_subZeroOctave)
    {
        //расчет начального значения sigma для -1 октавы
        double subSigma = (double)_initSigma / 2.0;
        _sigmas[0][0] = subSigma;
    }
    else _sigmas[0][0] = _initSigma;
    for (int octave = 1; octave < _octaves; octave++)
        _sigmas[octave][0] = _sigmas[octave - 1][0] * 2.0;
    //расчет значений sigma для подуровней
    for (int octave = 0; octave < _octaves; octave++)
        for (int level = 1; level < _levels + 2; level++)
            _sigmas[octave][level] = _sigmas[octave][level - 1] * quot;
}

void ImagePyramid::_buildPyramid(const SubImage &image, BorderMode mode)
{
    ImageTransformer transformer(mode);
    //заполнение начальных изображений в октавах
    if (_subZeroOctave)
    {
        //с учетом -1 октавы
        _pyramid[0][0] = transformer.resizedUp(image, 2);
        _buildOctave(0, mode);
        _pyramid[1][0] = image;
        _buildOctave(1, mode);
        //заполнение уровней в октавах
        for (int octave = 2; octave < _octaves; octave++)
        {
            _pyramid[octave][0] =
                    transformer.resizedDown(
                        _pyramid[octave - 1][_levels - 1], 2);
            _buildOctave(octave, mode);
        }
    }
    else
    {
        //без учета -1 октавы
        _pyramid[0][0] = image;
        _buildOctave(0, mode);
        //заполнение уровней в октавах
        for (int octave = 1; octave < _octaves; octave++)
        {
            _pyramid[octave][0] =
                    transformer.resizedDown(
                        _pyramid[octave - 1][_levels - 1], 2);
            _buildOctave(octave, mode);
        }
    }
}

void ImagePyramid::_buildOctave(int octave, BorderMode mode)
{
    ImageTransformer transformer(mode);
    for (int currLevel = 1; currLevel < _levels + 2; currLevel++)
    {
        //расчет Гауссова фильтра для свертки с предыдушим изображением
        //sigma = sqrt(currSigma^2 - prevSigma^2)
        double currSigma = _sigmas[octave][currLevel];
        double prevSigma = _sigmas[octave][currLevel - 1];
        double deltaSigma = std::sqrt(currSigma * currSigma -
                                      prevSigma * prevSigma);
        _pyramid[octave][currLevel] =
                transformer.gaussSmoothed(
                        _pyramid[octave][currLevel - 1], deltaSigma);
    }
}

void ImagePyramid::_buildDogPyramid()
{
    for (int octave = 0; octave < _octaves; octave++)
    {
        int width = _pyramid[octave][0]._width;
        int height = _pyramid[octave][0]._height;
        for (int level = 0; level < _levels + 1; level++)
        {
            //расчет элемента DoG
            SubImage dogItem(width, height);
            for (int index = 0; index < width * height; index++)
                dogItem._content[index] =
                        _pyramid[octave][level + 1]._content[index] -
                                _pyramid[octave][level]._content[index];
            _dogPyramid[octave][level] = dogItem;
        }
    }
}

bool ImagePyramid::_isDogExtremum(int x, int y, int octave, int level) const
{
    if (octave < 0 || octave >= _octaves) return false;
    if (level < 1 || level >= _levels) return false;
    bool maxFlag = true;
    bool minFlag = true;

    double etalonValue = _dogPyramid[octave][level].pixel(x, y);
    for (int dy = -1; dy < 2; dy++)
    {
        for (int dx = -1; dx < 2; dx++)
        {
            for (int dLevel = -1; dLevel < 2; dLevel++)
            {
                if (!(dLevel == 0 && dx == 0 && dy == 0))
                {
                    double currValue =
                            _dogPyramid[octave][level + dLevel].pixel(
                                x + dx, y + dy);
                    if (currValue > etalonValue) maxFlag = false;
                    if (currValue < etalonValue) minFlag = false;
                    if (!maxFlag && !minFlag) return false;
                }
            }
        }
    }
    if (minFlag && maxFlag) return false;
    return true;
}
