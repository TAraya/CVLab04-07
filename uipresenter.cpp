#include "imagebuilder.h"
#include "imagetransformer.h"
#include "poicorresponder.h"
#include "uipresenter.h"
#include <ctime>

UIPresenter::UIPresenter(MainWindow *view)
{
    _view = view;
}

UIPresenter::~UIPresenter()
{
    ;
}

bool UIPresenter::isLeftImageLoaded() const
{
    return _isLeftImageLoaded;
}

bool UIPresenter::isRightImageLoaded() const
{
    return _isRightImageLoaded;
}

void UIPresenter::loadLeftImage(QString path)
{
    clock_t start = clock();
    _view->writeInfo("Загрузка изображения");
    QImage image(path);
    _leftImage.loadImage(image);
    _isLeftImageLoaded = true;
    _leftPoiInfo.resetIsInit();
    _leftPyramid.init(_leftImage, pyramidLevels,
                      pyramidInitSigma, subZeroOctave, borderMode);
    if (_isRightImageLoaded)
        _view->drawImage(
            ImageBuilder::buildTwinImage(_leftImage, _rightImage).getQImage());
    else _view->drawImage(_leftImage.getQImage());
    QString durationInfo =
            QString("Изображение загружено, %1 с.")
                    .arg((double)(clock() - start) / CLOCKS_PER_SEC);
    _view->writeInfo(durationInfo);
    _view->setProcessControlsEnabled(true);
}

void UIPresenter::loadRightImage(QString path)
{
    clock_t start = clock();
    _view->writeInfo("Загрузка изображения");
    QImage image(path);
    _rightImage.loadImage(image);
    _isRightImageLoaded = true;
    _rightPoiInfo.resetIsInit();
    _rightPyramid.init(_rightImage, pyramidLevels,
                       pyramidInitSigma, subZeroOctave, borderMode);
    if (_isLeftImageLoaded)
        _view->drawImage(
            ImageBuilder::buildTwinImage(_leftImage, _rightImage).getQImage());
    else _view->drawImage(_rightImage.getQImage());
    QString durationInfo =
            QString("Изображение загружено, %1 с.")
                    .arg((double)(clock() - start) / CLOCKS_PER_SEC);
    _view->writeInfo(durationInfo);
    _view->setProcessControlsEnabled(true);
}

void UIPresenter::transformImage(TransformMode mode)
{
    if (!_isRightImageLoaded) return ;
    clock_t start = clock();
    _view->writeInfo("Преобразование изображения");
    ImageTransformer transformer(borderMode);
    switch (mode) {
    case TransformMode::TRANSFORM_BRIGHT:
        _rightImage =
                transformer.brighted(_rightImage, brightQuot, brightBias);
        break;
    case TransformMode::TRANSFORM_NOISE:
         _rightImage = transformer.noised(_rightImage, noiseQuot);
        if (brightIsNormalise) transformer.normalize(_rightImage);
        break;
    case TransformMode::TRANSFORM_BIAS:
        _rightImage = transformer.biased(_rightImage, biasAngle, false);
        break;
    case TransformMode::TRANSFORM_SCALE:
        _rightImage =
                transformer.scaled(_rightImage,
                                   scaleHorizontalQuot, scaleVerticalQuot);
        break;
    case TransformMode::TRANSFORM_ROTATE:
        _rightImage = transformer.rotated(_rightImage, rotateAngle, false);
        break;
    default:
        break;
    }
    if (_isLeftImageLoaded)
        _view->drawImage(
            ImageBuilder::buildTwinImage(_leftImage, _rightImage).getQImage());
    else _view->drawImage(_rightImage.getQImage());
    QString durationInfo =
            QString("Изображение преобразовано, %1 с.")
                    .arg((double)(clock() - start) / CLOCKS_PER_SEC);
    _rightPyramid.init(_rightImage, pyramidLevels,
                       pyramidInitSigma, subZeroOctave, borderMode);
    _view->writeInfo(durationInfo);
    _rightPoiInfo.resetIsInit();
}

void UIPresenter::processImage(ProcessMode mode)
{
    if (!_isLeftImageLoaded && !_isRightImageLoaded) return ;
    clock_t start = clock();
    _view->writeInfo("Обработка изображения");
    //установка параметров
    _leftPoiInfo.setHarrysParameters(harrysR, harrysSigma, harrysLocalMaxR,
                                     harrysTreshold, harrysMaskSigma,
                                     borderMode);
    _rightPoiInfo.setHarrysParameters(harrysR, harrysSigma, harrysLocalMaxR,
                                     harrysTreshold, harrysMaskSigma,
                                     borderMode);
    _leftPoiInfo.setDescriptorParameters(descRadius, descNetSize,
                                         descMaskSigma, descHystSize);
    _rightPoiInfo.setDescriptorParameters(descRadius, descNetSize,
                                          descMaskSigma, descHystSize);
    _leftPoiInfo.setOrientationParameters(orientRadius, orientMaskSigma,
                                          orientHystSize);
    _rightPoiInfo.setOrientationParameters(orientRadius, orientMaskSigma,
                                           orientHystSize);
    //методы обработки
    if (mode == ProcessMode::HARRYS_DETECTOR) _harrysDetectorProc();
    if (mode == ProcessMode::BLOB_DETECTOR) _blobDetectorProc();
    if (mode == ProcessMode::BASE_MATCH) _baseCorrespProc();
    if (mode == ProcessMode::ROTATION_MATCH) _rotatedCorrespProc();
    if (mode == ProcessMode::SCALE_MATCH) _scaleCorrespProc();
    if (mode == ProcessMode::SIFT_MATCH) _siftLikeCorrespProc();
    QString durationInfo =
            QString("Обработка завершена, %1 с.")
                    .arg((double)(clock() - start) / CLOCKS_PER_SEC);
    _view->writeInfo(durationInfo);
}

void UIPresenter::_harrysDetectorProc()
{
    if (_isLeftImageLoaded && _isRightImageLoaded)
    {
        _leftPoiInfo.initHarrysDetector(_leftImage);
        _rightPoiInfo.initHarrysDetector(_rightImage);
        int leftPoiCount = 0;
        vector<PointOfInterest> leftPoiVector =
                _leftPoiInfo.poiVector(-1, leftPoiCount);
        int rightPoiCount = 0;
        vector<PointOfInterest> rightPoiVector =
                _rightPoiInfo.poiVector(-1, rightPoiCount);
        SubImage leftPoiImage =
                ImageBuilder::buildImageWithPoi(_leftImage, leftPoiVector);
        SubImage rightPoiImage =
                ImageBuilder::buildImageWithPoi(_rightImage, rightPoiVector);
        SubImage image = ImageBuilder::buildTwinImage(leftPoiImage, rightPoiImage);
        _view->drawImage(image.getQImage());
    }
    if (_isLeftImageLoaded && !_isRightImageLoaded)
        _drawLeftPoiImageOnly();
    if (_isRightImageLoaded && !_isLeftImageLoaded)
        _drawRightPoiImageOnly();
}

void UIPresenter::_blobDetectorProc()
{
    if (_isLeftImageLoaded && _isRightImageLoaded)
    {
        _leftPoiInfo.initSiftDetector(_leftPyramid);
        _rightPoiInfo.initSiftDetector(_rightPyramid);
        _leftPoiInfo.calcPeekAngles(_leftPyramid);
        _rightPoiInfo.calcPeekAngles(_rightPyramid);
        int leftPoiCount = 0;
        vector<PointOfInterest> leftPoiVector =
                _leftPoiInfo.poiVector(-1, leftPoiCount);
        int rightPoiCount = 0;
        vector<PointOfInterest> rightPoiVector =
                _rightPoiInfo.poiVector(-1, rightPoiCount);
        SubImage leftBlobImage =
                ImageBuilder::buildImageWithBlob(_leftImage, leftPoiVector,
                                                 2 * descRadius, pyramidInitSigma);
        SubImage rightBlobImage =
                ImageBuilder::buildImageWithBlob(_rightImage, rightPoiVector,
                                                 2 * descRadius, pyramidInitSigma);
        SubImage image =
                ImageBuilder::buildTwinImage(leftBlobImage, rightBlobImage);
        _view->drawImage(image.getQImage());
    }
    if (_isLeftImageLoaded && !_isRightImageLoaded)
        _drawLeftBlobImageOnly();
    if (_isRightImageLoaded && !_isLeftImageLoaded)
        _drawRightBlobImageOnly();
}

void UIPresenter::_baseCorrespProc()
{
    if (_isLeftImageLoaded && _isRightImageLoaded)
    {
        _leftPoiInfo.initHarrysDetector(_leftImage);
        _rightPoiInfo.initHarrysDetector(_rightImage);
        _leftPoiInfo.calcDescriptors(_leftImage, false, normalizeMode);
        _rightPoiInfo.calcDescriptors(_rightImage, false, normalizeMode);
        PoiCorresponder matcher(correspNNDmax, correspLmax);
        matcher.init(_leftPoiInfo, _rightPoiInfo);
        SubImage image =
            ImageBuilder::buildTwinImage(_leftImage, _rightImage,
                                         matcher, true);
        _view->drawImage(image.getQImage());
    }
    if (_isLeftImageLoaded && !_isRightImageLoaded)
        _drawLeftPoiImageOnly();
    if (_isRightImageLoaded && !_isLeftImageLoaded)
        _drawRightPoiImageOnly();
}

void UIPresenter::_rotatedCorrespProc()
{
    if (_isLeftImageLoaded && _isRightImageLoaded)
    {
        _leftPoiInfo.initHarrysDetector(_leftImage);
        _rightPoiInfo.initHarrysDetector(_rightImage);
        _leftPoiInfo.calcPeekAngles(_leftImage);
        _rightPoiInfo.calcPeekAngles(_rightImage);
        _leftPoiInfo.calcDescriptors(_leftImage, true, normalizeMode);
        _rightPoiInfo.calcDescriptors(_rightImage, true, normalizeMode);
        PoiCorresponder matcher(correspNNDmax, correspLmax);
        matcher.init(_leftPoiInfo, _rightPoiInfo);
        SubImage image =
            ImageBuilder::buildTwinImage(_leftImage, _rightImage,
                                         matcher, true);
        _view->drawImage(image.getQImage());
    }
    if (_isLeftImageLoaded && !_isRightImageLoaded)
        _drawLeftPoiImageOnly();
    if (_isRightImageLoaded && !_isLeftImageLoaded)
        _drawRightPoiImageOnly();
}

void UIPresenter::_scaleCorrespProc()
{
    if (_isLeftImageLoaded && _isRightImageLoaded)
    {
        _leftPoiInfo.initSiftDetector(_leftPyramid);
        _rightPoiInfo.initSiftDetector(_rightPyramid);
        _leftPoiInfo.calcDescriptors(_leftPyramid, false, normalizeMode);
        _rightPoiInfo.calcDescriptors(_rightPyramid, false, normalizeMode);
        PoiCorresponder matcher(correspNNDmax, correspLmax);
        matcher.init(_leftPoiInfo, _rightPoiInfo);
        SubImage image =
            ImageBuilder::buildTwinImage(_leftImage, _rightImage, matcher,
                                       false, 2 * descRadius, pyramidInitSigma);
        _view->drawImage(image.getQImage());
    }
    if (_isLeftImageLoaded && !_isRightImageLoaded)
        _drawLeftBlobImageOnly();
    if (_isRightImageLoaded && !_isLeftImageLoaded)
        _drawRightBlobImageOnly();
}

void UIPresenter::_siftLikeCorrespProc()
{
    if (_isLeftImageLoaded && _isRightImageLoaded)
    {
        _leftPoiInfo.initSiftDetector(_leftPyramid);
        _rightPoiInfo.initSiftDetector(_rightPyramid);
        _leftPoiInfo.calcPeekAngles(_leftPyramid);
        _rightPoiInfo.calcPeekAngles(_rightPyramid);
        _leftPoiInfo.calcDescriptors(_leftPyramid, true, normalizeMode);
        _rightPoiInfo.calcDescriptors(_rightPyramid, true, normalizeMode);
        PoiCorresponder matcher(correspNNDmax, correspLmax);
        matcher.init(_leftPoiInfo, _rightPoiInfo);
        SubImage image =
            ImageBuilder::buildTwinImage(_leftImage, _rightImage, matcher,
                                       false, 2 * descRadius, pyramidInitSigma);
        _view->drawImage(image.getQImage());
    }
    if (_isLeftImageLoaded && !_isRightImageLoaded)
        _drawLeftBlobImageOnly();
    if (_isRightImageLoaded && !_isLeftImageLoaded)
        _drawRightBlobImageOnly();
}

void UIPresenter::_drawLeftPoiImageOnly()
{
    _leftPoiInfo.initHarrysDetector(_leftImage);
    int poiCount = 0;
    vector<PointOfInterest> poiVector =
            _leftPoiInfo.poiVector(-1, poiCount);
    SubImage image =
            ImageBuilder::buildImageWithPoi(_leftImage, poiVector);
    _view->drawImage(image.getQImage());
}

void UIPresenter::_drawRightPoiImageOnly()
{
    _rightPoiInfo.initHarrysDetector(_rightImage);
    int poiCount = 0;
    vector<PointOfInterest> poiVector =
            _rightPoiInfo.poiVector(-1, poiCount);
    SubImage image =
            ImageBuilder::buildImageWithPoi(_rightImage, poiVector);
    _view->drawImage(image.getQImage());
}

void UIPresenter::_drawLeftBlobImageOnly()
{
    _leftPoiInfo.initSiftDetector(_leftPyramid);
    _leftPoiInfo.calcPeekAngles(_leftPyramid);
    int poiCount = 0;
    vector<PointOfInterest> poiVector =
            _leftPoiInfo.poiVector(-1, poiCount);
    SubImage image =
            ImageBuilder::buildImageWithBlob(_leftImage, poiVector,
                                             2 * descRadius, pyramidInitSigma);
    _view->drawImage(image.getQImage());
}

void UIPresenter::_drawRightBlobImageOnly()
{
    _rightPoiInfo.initSiftDetector(_rightPyramid);
    _rightPoiInfo.calcPeekAngles(_rightPyramid);
    int poiCount = 0;
    vector<PointOfInterest> poiVector =
            _rightPoiInfo.poiVector(-1, poiCount);
    SubImage image =
            ImageBuilder::buildImageWithBlob(_rightImage, poiVector,
                                             2 * descRadius, pyramidInitSigma);
    _view->drawImage(image.getQImage());
}
