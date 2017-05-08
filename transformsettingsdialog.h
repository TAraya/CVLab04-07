#ifndef TRANSFORMSETTINGSDIALOG_H
#define TRANSFORMSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class TransformSettingsDialog;
}

class TransformSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransformSettingsDialog(QWidget *parent = 0);
    ~TransformSettingsDialog();

    //получение параметров
    double biasAngle();
    double brightBias();
    bool brightNormalize();
    double brightQuot();
    double noiseQuot();
    double rotateAngle();
    double scaleHQuot();
    double scaleVQuot();
    //установка параметров
    void setBiasAngle(double value);
    void setBrightBias(double value);
    void setBrightNormalize(bool value);
    void setBrightQuot(double value);
    void setNoiseQuot(double value);
    void setRotateAngle(double value);
    void setScaleHQuot(double value);
    void setScaleVQuot(double value);

private:
    Ui::TransformSettingsDialog *ui;
};

#endif // TRANSFORMSETTINGSDIALOG_H
