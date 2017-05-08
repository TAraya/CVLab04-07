#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _presenter = new UIPresenter(this);
    ui->processTypeComboBox->addItem("Детектор Харриса");
    ui->processTypeComboBox->addItem("Детектор блобов DoG");
    ui->processTypeComboBox->addItem("Базовое сопоставление");
    ui->processTypeComboBox->addItem("Инвариантное ко вращению сопоставление");
    ui->processTypeComboBox->addItem("Инвариантное к масштабу сопоставление");
    ui->processTypeComboBox->addItem("Сопоставление на основе подхода SIFT");
    ui->statusBar->showMessage("Для начала работы загрузите изображения");
    setProcessControlsEnabled(false);
}

MainWindow::~MainWindow()
{
    delete _presenter;
    delete ui;
}

void MainWindow::drawImage(const QImage &image)
{
    ui->imageLabel->setScaledContents(false);
    QPixmap pixmap = QPixmap::fromImage(image);
    ui->imageLabel->setPixmap(pixmap);
}

void MainWindow::writeInfo(const QString &info)
{
    ui->statusBar->showMessage(info);
}

void MainWindow::setProcessControlsEnabled(bool value)
{
    ui->processPushButton->setEnabled(value);
    ui->biasTransformAction->setEnabled(value);
    ui->brightTransformAction->setEnabled(value);
    ui->noiseTransformAction->setEnabled(value);
    ui->rotateTransformAction->setEnabled(value);
    ui->scaleTransformAction->setEnabled(value);
}

void MainWindow::showTransformSettingsDialog()
{
    TransformSettingsDialog *dialog = new TransformSettingsDialog(this);
    dialog->setBiasAngle(_presenter->biasAngle);
    dialog->setBrightBias(_presenter->brightBias);
    dialog->setBrightNormalize(_presenter->brightIsNormalise);
    dialog->setBrightQuot(_presenter->brightQuot);
    dialog->setNoiseQuot(_presenter->noiseQuot);
    dialog->setRotateAngle(_presenter->rotateAngle);
    dialog->setScaleHQuot(_presenter->scaleHorizontalQuot);
    dialog->setScaleVQuot(_presenter->scaleVerticalQuot);

    if (dialog->exec() == QDialog::Accepted)
    {
        _presenter->biasAngle = dialog->biasAngle();
        _presenter->brightBias = dialog->brightBias();
        _presenter->brightIsNormalise = dialog->brightNormalize();
        _presenter->brightQuot = dialog->brightQuot();
        _presenter->noiseQuot = dialog->noiseQuot();
        _presenter->rotateAngle = dialog->rotateAngle();
        _presenter->scaleHorizontalQuot = dialog->scaleHQuot();
        _presenter->scaleVerticalQuot = dialog->scaleVQuot();
    }
    delete dialog;
}

void MainWindow::showHarrysSettingsDialog()
{
    HarrysSettingsDialog *dialog = new HarrysSettingsDialog(this);

    dialog->setHarrysLocalMaxRadius(_presenter->harrysLocalMaxR);
    dialog->setHarrysMaskSigma(_presenter->harrysMaskSigma);
    dialog->setHarrysRadius(_presenter->harrysR);
    dialog->setHarrysSigma(_presenter->harrysSigma);
    dialog->setHarrysTreshold(_presenter->harrysTreshold);

    if (dialog->exec() == QDialog::Accepted)
    {
        _presenter->harrysLocalMaxR = dialog->harrysLocalMaxRadius();
        _presenter->harrysMaskSigma = dialog->harrysMaskSigma();
        _presenter->harrysR = dialog->harrysRadius();
        _presenter->harrysSigma = dialog->harrysSigma();
        _presenter->harrysTreshold = dialog->harrysTreshold();
    }
    delete dialog;
}

void MainWindow::showDescriptorSettingsDialog()
{
    DescriptorSettingsDialog *dialog = new DescriptorSettingsDialog(this);

    dialog->setDescRadius(_presenter->descRadius);
    dialog->setDescNetSize(_presenter->descNetSize);
    dialog->setDescMaskSigma(_presenter->descMaskSigma);
    dialog->setDescHystSize(_presenter->descHystSize);
    dialog->setOrientRadius(_presenter->orientRadius);
    dialog->setOrientMaskSigma(_presenter->orientMaskSigma);
    dialog->setOrientHystSize(_presenter->orientHystSize);
    dialog->setCorrespNNDmax(_presenter->correspNNDmax);
    dialog->setCorrespLmax(_presenter->correspLmax);
    dialog->setPyramidInitSigma(_presenter->pyramidInitSigma);
    dialog->setPyramidLevels(_presenter->pyramidLevels);

    if (dialog->exec() == QDialog::Accepted)
    {
        _presenter->descRadius = dialog->descRadius();
        _presenter->descNetSize = dialog->descNetSize();
        _presenter->descMaskSigma = dialog->descMaskSigma();
        _presenter->descHystSize = dialog->descHystSize();
        _presenter->orientRadius = dialog->orientRaduis();
        _presenter->orientMaskSigma = dialog->orientMaskSigma();
        _presenter->orientHystSize = dialog->orientHystSize();
        _presenter->correspNNDmax = dialog->correspNNDmax();
        _presenter->correspLmax = dialog->correspLmax();
        _presenter->pyramidInitSigma = dialog->pyramidInitSigma();
        _presenter->pyramidLevels = dialog->pyramidLevels();
    }
    delete dialog;
}

void MainWindow::on_openImageAction_triggered()
{
    QString path = QFileDialog::getOpenFileName(
                this, tr("Выбор изображения"), "",
                tr("Файлы изображений (*.bmp *.gif *.jpg *.jpeg *png *tiff)"));
    if (path != "") _presenter->loadLeftImage(path);
}

void MainWindow::on_processPushButton_clicked()
{
        ProcessMode mode = ProcessMode::HARRYS_DETECTOR;
        if (ui->processTypeComboBox->currentText() ==
                "Детектор блобов DoG")
            mode = ProcessMode::BLOB_DETECTOR;
        if (ui->processTypeComboBox->currentText() ==
                "Базовое сопоставление")
            mode = ProcessMode::BASE_MATCH;
        if (ui->processTypeComboBox->currentText() ==
                "Инвариантное ко вращению сопоставление")
            mode = ProcessMode::ROTATION_MATCH;
        if (ui->processTypeComboBox->currentText() ==
                "Инвариантное к масштабу сопоставление")
            mode = ProcessMode::SCALE_MATCH;
        if (ui->processTypeComboBox->currentText() ==
                "Сопоставление на основе подхода SIFT")
            mode = ProcessMode::SIFT_MATCH;
        _presenter->processImage(mode);
}

void MainWindow::on_scaleTransformAction_triggered()
{
    _presenter->transformImage(TransformMode::TRANSFORM_SCALE);
}

void MainWindow::on_rotateTransformAction_triggered()
{
    _presenter->transformImage(TransformMode::TRANSFORM_ROTATE);
}

void MainWindow::on_noiseTransformAction_triggered()
{
    _presenter->transformImage(TransformMode::TRANSFORM_NOISE);
}

void MainWindow::on_brightTransformAction_triggered()
{
    _presenter->transformImage(TransformMode::TRANSFORM_BRIGHT);
}

void MainWindow::on_biasTransformAction_triggered()
{
    _presenter->transformImage(TransformMode::TRANSFORM_BIAS);
}

void MainWindow::on_setBorderBlackAction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->borderMode = BorderMode::BORDER_BLACK;
        ui->setBorderCopyAction->setChecked(false);
        ui->setBorderMirrorAction->setChecked(false);
        ui->setBorderCycleAction->setChecked(false);
    }
    else
    {
        ui->setBorderBlackAction->setChecked(true);
    }
}

void MainWindow::on_setBorderCopyAction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->borderMode = BorderMode::BORDER_COPY;
        ui->setBorderBlackAction->setChecked(false);
        ui->setBorderMirrorAction->setChecked(false);
        ui->setBorderCycleAction->setChecked(false);
    }
    else
    {
        ui->setBorderCopyAction->setChecked(true);
    }
}

void MainWindow::on_setBorderMirrorAction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->borderMode = BorderMode::BORDER_MIRROR;
        ui->setBorderBlackAction->setChecked(false);
        ui->setBorderCopyAction->setChecked(false);
        ui->setBorderCycleAction->setChecked(false);
    }
    else
    {
        ui->setBorderMirrorAction->setChecked(true);
    }
}

void MainWindow::on_setBorderCycleAction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->borderMode = BorderMode::BORDER_CYCLE;
        ui->setBorderBlackAction->setChecked(false);
        ui->setBorderMirrorAction->setChecked(false);
        ui->setBorderCopyAction->setChecked(false);
    }
    else
    {
        ui->setBorderCycleAction->setChecked(true);
    }
}

void MainWindow::on_quitAction_triggered()
{
    this->close();
}

void MainWindow::on_transformSettingsAction_triggered()
{
    showTransformSettingsDialog();
}

void MainWindow::on_setNoNormalizationAction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->normalizeMode = NormalizeMode::NO_NORMALIZE;
        ui->setNoNormalizationAction->setChecked(true);
        ui->setSingleNormalizationAction->setChecked(false);
        ui->setComplexNormalizationAction->setChecked(false);
        ui->setComplexTrimNormalizationAaction->setChecked(false);
    }
    else
    {
        ui->setNoNormalizationAction->setChecked(true);
    }
}

void MainWindow::on_setSingleNormalizationAction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->normalizeMode = NormalizeMode::SINGLE_NORMALIZE;
        ui->setNoNormalizationAction->setChecked(false);
        ui->setSingleNormalizationAction->setChecked(true);
        ui->setComplexNormalizationAction->setChecked(false);
        ui->setComplexTrimNormalizationAaction->setChecked(false);
    }
    else
    {
        ui->setSingleNormalizationAction->setChecked(true);
    }
}

void MainWindow::on_setComplexNormalizationAction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->normalizeMode = NormalizeMode::COMPLEX_NORMALIZE;
        ui->setNoNormalizationAction->setChecked(false);
        ui->setSingleNormalizationAction->setChecked(false);
        ui->setComplexNormalizationAction->setChecked(true);
        ui->setComplexTrimNormalizationAaction->setChecked(false);
    }
    else
    {
        ui->setComplexNormalizationAction->setChecked(true);
    }
}

void MainWindow::on_openLeftImageAction_triggered()
{
    QString path = QFileDialog::getOpenFileName(
                this, tr("Выбор изображения"), "",
                tr("Файлы изображений (*.bmp *.gif *.jpg *.jpeg *png *tiff)"));
    if (path != "") _presenter->loadLeftImage(path);
}

void MainWindow::on_openRightImageAction_triggered()
{
    QString path = QFileDialog::getOpenFileName(
                this, tr("Выбор изображения"), "",
                tr("Файлы изображений (*.bmp *.gif *.jpg *.jpeg *png *tiff)"));
    if (path != "") _presenter->loadRightImage(path);
}

void MainWindow::on_descriptorsSettingsAction_triggered()
{
    showDescriptorSettingsDialog();
}

void MainWindow::on_harrysDetectorSettingsAction_triggered()
{
    showHarrysSettingsDialog();
}

void MainWindow::on_setComplexTrimNormalizationAaction_triggered(bool checked)
{
    if (checked)
    {
        _presenter->normalizeMode = NormalizeMode::COMPLEX_TRIM_NORMALIZE;
        ui->setNoNormalizationAction->setChecked(false);
        ui->setSingleNormalizationAction->setChecked(false);
        ui->setComplexNormalizationAction->setChecked(false);
        ui->setComplexTrimNormalizationAaction->setChecked(true);
    }
    else
    {
        ui->setComplexNormalizationAction->setChecked(true);
    }
}

void MainWindow::on_subZeroOctaveAction_triggered(bool checked)
{
    _presenter->subZeroOctave = checked;
}
