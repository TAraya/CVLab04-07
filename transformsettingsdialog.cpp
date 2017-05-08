#include "transformsettingsdialog.h"
#include "ui_transformsettingsdialog.h"

TransformSettingsDialog::TransformSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransformSettingsDialog)
{
    ui->setupUi(this);
}

TransformSettingsDialog::~TransformSettingsDialog()
{
    delete ui;
}

double TransformSettingsDialog::biasAngle()
{
    return ui->biasAngleSpinBox->value();
}

double TransformSettingsDialog::brightBias()
{
    return ui->brightBiasSpinBox->value();
}

bool TransformSettingsDialog::brightNormalize()
{
    return ui->brightNormaliseCheckBox->isChecked();
}

double TransformSettingsDialog::brightQuot()
{
    return ui->brightQuotSpinBox->value();
}

double TransformSettingsDialog::noiseQuot()
{
    return ui->noiseQuotSpinBox->value();
}

double TransformSettingsDialog::rotateAngle()
{
    return ui->rotateAngleSpinBox->value();
}

double TransformSettingsDialog::scaleHQuot()
{
    return ui->scaleHQuotSpinBox->value();
}

double TransformSettingsDialog::scaleVQuot()
{
    return ui->scaleVQuotSpinBox->value();
}

void TransformSettingsDialog::setBiasAngle(double value)
{
    ui->biasAngleSpinBox->setValue(value);
}

void TransformSettingsDialog::setBrightBias(double value)
{
    ui->brightBiasSpinBox->setValue(value);
}

void TransformSettingsDialog::setBrightNormalize(bool value)
{
    ui->brightNormaliseCheckBox->setChecked(value);
}

void TransformSettingsDialog::setBrightQuot(double value)
{
    ui->brightQuotSpinBox->setValue(value);
}

void TransformSettingsDialog::setNoiseQuot(double value)
{
    ui->noiseQuotSpinBox->setValue(value);
}

void TransformSettingsDialog::setRotateAngle(double value)
{
    ui->rotateAngleSpinBox->setValue(value);
}

void TransformSettingsDialog::setScaleHQuot(double value)
{
    ui->scaleHQuotSpinBox->setValue(value);
}

void TransformSettingsDialog::setScaleVQuot(double value)
{
    ui->scaleVQuotSpinBox->setValue(value);
}
