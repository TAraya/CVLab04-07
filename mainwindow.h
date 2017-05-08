#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "harryssettingsdialog.h"
#include "transformsettingsdialog.h"
#include "descriptorsettingsdialog.h"
#include <QMainWindow>
#include "uipresenter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //отрисовка изображения
    void drawImage(const QImage &image);
    //вывод информации в строку состояния
    void writeInfo(const QString &info);

    //блокировка/разблокировка элементов управления
    void setProcessControlsEnabled(bool value);

    //установка параметров
    void showTransformSettingsDialog();
    void showHarrysSettingsDialog();
    void showDescriptorSettingsDialog();

private slots:
    void on_openImageAction_triggered();
    void on_processPushButton_clicked();
    void on_scaleTransformAction_triggered();
    void on_rotateTransformAction_triggered();
    void on_noiseTransformAction_triggered();
    void on_brightTransformAction_triggered();
    void on_biasTransformAction_triggered();
    void on_setBorderBlackAction_triggered(bool checked);
    void on_setBorderCopyAction_triggered(bool checked);
    void on_setBorderMirrorAction_triggered(bool checked);
    void on_setBorderCycleAction_triggered(bool checked);
    void on_quitAction_triggered();
    void on_transformSettingsAction_triggered();
    void on_setSingleNormalizationAction_triggered(bool checked);
    void on_setComplexNormalizationAction_triggered(bool checked);
    void on_setNoNormalizationAction_triggered(bool checked);
    void on_openLeftImageAction_triggered();
    void on_openRightImageAction_triggered();
    void on_descriptorsSettingsAction_triggered();
    void on_harrysDetectorSettingsAction_triggered();
    void on_setComplexTrimNormalizationAaction_triggered(bool checked);


    void on_subZeroOctaveAction_triggered(bool checked);

private:
    Ui::MainWindow *ui;
    UIPresenter *_presenter;
};

#endif // MAINWINDOW_H
