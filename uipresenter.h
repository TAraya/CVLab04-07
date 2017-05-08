#ifndef UIPRESENTER_H
#define UIPRESENTER_H

class UIPresenter;

#include "enumerations.h"
#include "subimage.h"
#include "mainwindow.h"
#include "poiinfo.h"
#include <QImage>

class MainWindow;

class UIPresenter
{
public:
    UIPresenter(MainWindow *view);
    ~UIPresenter();

    BorderMode borderMode = BorderMode::BORDER_COPY;
    NormalizeMode normalizeMode = NormalizeMode::COMPLEX_NORMALIZE;
    TransformMode transformMode = TransformMode::TRANSFORM_NOISE;

    double scaleHorizontalQuot = 0.75;
    double scaleVerticalQuot = 1.25;
    double biasAngle = 15.0;
    double rotateAngle = 15.0;
    double noiseQuot = 0.1;
    double brightQuot = 1.4;
    double brightBias = 0.1;
    bool brightIsNormalise = true;

    int harrysR = 5;
    double harrysSigma = 1.41;
    int harrysLocalMaxR = 4;
    double harrysTreshold = 0.20;
    double harrysMaskSigma = 2.0;

    int descRadius = 8;
    int descNetSize = 4;
    double descMaskSigma = 4.0;
    int descHystSize = 8;

    int orientRadius = 7;
    double orientMaskSigma = 3.0;
    int orientHystSize = 36;

    double correspNNDmax = 0.80;
    double correspLmax = 1000.0;

    bool subZeroOctave = false;
    double pyramidInitSigma = 1.6;
    int pyramidLevels = 5;

    bool isLeftImageLoaded() const;
    bool isRightImageLoaded() const;

    void loadLeftImage(QString path);
    void loadRightImage(QString path);
    void transformImage(TransformMode mode);
    void processImage(ProcessMode mode);

private:
    bool _isLeftImageLoaded = false;
    bool _isRightImageLoaded = false;

    MainWindow *_view;
    SubImage _leftImage;
    SubImage _rightImage;
    ImagePyramid _leftPyramid;
    ImagePyramid _rightPyramid;
    PoiInfo _leftPoiInfo;
    PoiInfo _rightPoiInfo;

    void _harrysDetectorProc();
    void _blobDetectorProc();
    void _baseCorrespProc();
    void _rotatedCorrespProc();
    void _scaleCorrespProc();
    void _siftLikeCorrespProc();

    void _drawLeftPoiImageOnly();
    void _drawRightPoiImageOnly();
    void _drawLeftBlobImageOnly();
    void _drawRightBlobImageOnly();
};

#endif // UIPRESENTER_H
