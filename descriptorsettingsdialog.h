#ifndef DESCRIPTORSETTINGSDIALOG_H
#define DESCRIPTORSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class DescriptorSettingsDialog;
}

class DescriptorSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DescriptorSettingsDialog(QWidget *parent = 0);
    ~DescriptorSettingsDialog();

    //получение параметров
    int descRadius();
    int descNetSize();
    double descMaskSigma();
    int descHystSize();

    int orientRaduis();
    double orientMaskSigma();
    int orientHystSize();

    double correspNNDmax();
    double correspLmax();

    double pyramidInitSigma();
    int pyramidLevels();

    //установка параметров
    void setDescRadius(int value);
    void setDescNetSize(int value);
    void setDescMaskSigma(double value);
    void setDescHystSize(int value);

    void setOrientRadius(int value);
    void setOrientMaskSigma(double value);
    void setOrientHystSize(int value);

    void setCorrespNNDmax(double value);
    void setCorrespLmax(double value);

    void setPyramidInitSigma(double value);
    void setPyramidLevels(int value);

private:
    Ui::DescriptorSettingsDialog *ui;
};

#endif // DESCRIPTORSETTINGSDIALOG_H
