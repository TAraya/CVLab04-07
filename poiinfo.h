#ifndef POIINFO_H
#define POIINFO_H
#include "enumerations.h"
#include "imagepyramid.h"
#include "pointofinterest.h"
#include "subimage.h"
#include <utility>

using namespace std;

//базовый класс для хранения и работы с точками интереса в изображении
//также хранит настройки для инициализации от изображения и расчета дескр.
class PoiInfo
{
public:
    PoiInfo();
    ~PoiInfo();
    //признак произведенной инициализации (нахождения точек интереса)
    bool isInit() const;
    //признак рассчитанных дескрипторов
    bool isDescriptorsCalc() const;
    //признак установки параметров поиска точек интереса
    bool isHarrysParametersSetted() const;
    //признак установки параметров расчета дескрипторов
    bool isDescriptorParametersSetted() const;
    //признак установки параметров расчета ориентации дескрипторов
    bool isOrientationParametersSetted() const;
    //устанавливает параметры поиска точек интереса
    void setHarrysParameters(int radius, double sigma, int localMaxRadius,
                             double treshold, double maskSigma,
                             BorderMode mode);
    //устанавливает параметры расчета дескрипторов
    void setDescriptorParameters(int descRadius, int descNetSize,
                                 double descMaskSigma, int descHystSize);
    //устанавливает параметры расчета ориентации дескрипторов
    void setOrientationParameters(int orientRadius, double orientMaskSigma,
                                  int orientHystSize);
    //метод поиска точек интереса в изображении
    void initHarrysDetector(const SubImage &image);
    void initSiftDetector(const ImagePyramid &pyramid);
    //метод расчета углов пиков и возможного добавления новых точек интереса
    void calcPeekAngles(const SubImage &image);
    void calcPeekAngles(const ImagePyramid &pyramid);
    //расчет дескрипторов
    void calcDescriptors(const SubImage &image,
                         bool consRotate, NormalizeMode mode);
    void calcDescriptors(const ImagePyramid &pyramid,
                         bool consRotate, NormalizeMode mode);
    //возвращает общее число найденных точек интереса
    int poiCount() const;
    //возвращает вектор из не более чем poiMaxCount точек интереса
    vector<PointOfInterest> poiVector(int poiMaxCount, int &poiCount) const;
    //обнуляет инициализацию
    void resetIsInit();

protected:
    //пременные состояния
    bool _isHarrysParametersSetted;
    bool _isDescriptorParametersSetted;
    bool _isOrientationParametersSetted;
    bool _isInit;
    bool _isDescriptorsCalc;
    int _poiCount;
    vector<PointOfInterest> _poiVector;
    BorderMode _borderMode;
    NormalizeMode _normalizeMode;
    double _diameter;
    int _harrysRadius;
    double _harrysSigma;
    int _harrysLocalMaxRadius;
    double _harrysTreshold;
    double _harrysMaskSigma;
    int _descRadius;
    int _descNetSize;
    double _descMaskSigma;
    int _descHystSize;
    int _orientRadius;
    double _orientMaskSigma;
    int _orientHystSize;
    double _blobHarrysLimit = 0.03;
    //коэффициент для устойчивости фильтрации точек интереса
    double _localMaxPoiQuot = 1.0;
    //подсчитывает число точек интереса с целевой функцией большей,
    //чем у точек с целевой функцией в окрестности радиусом radius
    //изменяя маску mask и с учетом коэффициента limitQuot
    int _localMaxPoiCoutAtRadius(double radius, vector<bool> &mask) const;
    //обнуление значенй, не являющихся локальными максимумами в представлении
    void _localMaxOnly(SubImage &poiImage, double radius,
                       BorderMode mode) const;
    //определяет, является ли точка локальным максимумом в представлении
    bool _isLocalMax(SubImage &poiImage, int x, int y, double radius,
                     BorderMode mode) const;
    //вспомогательный метод подсчета целевой функции
    double _harrysValue(const SubImage &image, int x, int y);
    static double _getHarrysError(double a, double b, double c);

    static double _abs(double x);

    double _epsilon = 0.000000001;
};

#endif // POIINFO_H
