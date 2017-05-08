#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

//перечисление режима обработки краев изображения
enum class BorderMode
{
    BORDER_BLACK,
    BORDER_COPY,
    BORDER_MIRROR,
    BORDER_CYCLE
};

//перечисление выбранной стратегии нормализации гистограмм
enum class NormalizeMode
{
    NO_NORMALIZE,
    SINGLE_NORMALIZE,
    COMPLEX_NORMALIZE,
    COMPLEX_TRIM_NORMALIZE
};

//перечисление выбранного режима обработки
enum class ProcessMode
{
    HARRYS_DETECTOR,
    BLOB_DETECTOR,
    BASE_MATCH,
    ROTATION_MATCH,
    SCALE_MATCH,
    SIFT_MATCH
};

//перечисление выбранного режима преобразования изображения
enum class TransformMode
{
    TRANSFORM_NOISE,
    TRANSFORM_BRIGHT,
    TRANSFORM_SCALE,
    TRANSFORM_BIAS,
    TRANSFORM_ROTATE
};

#endif // ENUMERATIONS_H
