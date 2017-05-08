#include "harryssettingsdialog.h"
#include "ui_harryssettingsdialog.h"

HarrysSettingsDialog::HarrysSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HarrysSettingsDialog)
{
    ui->setupUi(this);
}

HarrysSettingsDialog::~HarrysSettingsDialog()
{
    delete ui;
}

int HarrysSettingsDialog::harrysRadius()
{
    return ui->harrysRadiusSpinBox->value();
}

int HarrysSettingsDialog::harrysLocalMaxRadius()
{
    return ui->harrysLocalMaxRadiusSpinBox->value();
}

double HarrysSettingsDialog::harrysSigma()
{
    return ui->harrysSigmaSpinBox->value();
}

double HarrysSettingsDialog::harrysTreshold()
{
    return ui->harrysTresholdSpinBox->value();
}

double HarrysSettingsDialog::harrysMaskSigma()
{
    return ui->harrysMaskSigmaSpinBox->value();
}

void HarrysSettingsDialog::setHarrysRadius(int value)
{
    ui->harrysRadiusSpinBox->setValue(value);
}

void HarrysSettingsDialog::setHarrysLocalMaxRadius(int value)
{
    ui->harrysLocalMaxRadiusSpinBox->setValue(value);
}

void HarrysSettingsDialog::setHarrysSigma(double value)
{
    ui->harrysSigmaSpinBox->setValue(value);
}

void HarrysSettingsDialog::setHarrysTreshold(double value)
{
    ui->harrysTresholdSpinBox->setValue(value);
}

void HarrysSettingsDialog::setHarrysMaskSigma(double value)
{
    ui->harrysMaskSigmaSpinBox->setValue(value);
}
