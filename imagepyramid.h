#ifndef IMAGEPYRAMID_H
#define IMAGEPYRAMID_H

#include "subimage.h"

class ImagePyramid
{
public:
    ImagePyramid();
    ~ImagePyramid();

    void init(const SubImage &image, int levels, double initSigma,
              bool subZeroOctave, BorderMode mode);

    bool isInit() const;
    bool subZeroOctave() const;
    int levels() const;
    int octaves() const;
    double initSigma() const;
    int pyramidSize() const;
    int dogPyramidSize() const;
    void nearestImageIndexes(double sigma, int &octave, int &level) const;

    friend class PoiInfo;
    friend class UIPresenter;
private:

    bool _isInit;
    bool _subZeroOctave;
    int _levels;
    int _octaves;
    double _initSigma;

    vector<vector<double>> _sigmas;
    vector<vector<SubImage>> _pyramid;
    vector<vector<SubImage>> _dogPyramid;

    void _countOctaves(int width, int height);
    void _calcSigmas();
    void _buildPyramid(const SubImage &image, BorderMode mode);
    void _buildOctave(int octave, BorderMode mode);
    void _buildDogPyramid();

    bool _isDogExtremum(int x, int y, int octave, int level)
    const;

    double _epsilon = 0.000000001;
};

#endif // IMAGEPYRAMID_H
