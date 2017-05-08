#ifndef HARRYSSETTINGSDIALOG_H
#define HARRYSSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class HarrysSettingsDialog;
}

class HarrysSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HarrysSettingsDialog(QWidget *parent = 0);
    ~HarrysSettingsDialog();

    //получение параметров
    int harrysRadius();
    int harrysLocalMaxRadius();
    double harrysSigma();
    double harrysTreshold();
    double harrysMaskSigma();
    //установка параметров
    void setHarrysRadius(int value);
    void setHarrysLocalMaxRadius(int value);
    void setHarrysSigma(double value);
    void setHarrysTreshold(double value);
    void setHarrysMaskSigma(double value);

private:
    Ui::HarrysSettingsDialog *ui;
};

#endif // HARRYSSETTINGSDIALOG_H
