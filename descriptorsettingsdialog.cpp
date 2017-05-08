#include "descriptorsettingsdialog.h"
#include "ui_descriptorsettingsdialog.h"

DescriptorSettingsDialog::DescriptorSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DescriptorSettingsDialog)
{
    ui->setupUi(this);
}

DescriptorSettingsDialog::~DescriptorSettingsDialog()
{
    delete ui;
}

int DescriptorSettingsDialog::descRadius()
{
    return ui->descRadiusSpinBox->value();
}

int DescriptorSettingsDialog::descNetSize()
{
    return ui->descNetSizeSpinBox->value();
}

double DescriptorSettingsDialog::descMaskSigma()
{
    return ui->descMaskSigmaSpinBox->value();
}

int DescriptorSettingsDialog::descHystSize()
{
    return ui->descHystSpinBox->value();
}

int DescriptorSettingsDialog::orientRaduis()
{
    return ui->orientRadiusSpinBox->value();
}

double DescriptorSettingsDialog::orientMaskSigma()
{
    return ui->orientMaskSigmaSpinBox->value();
}

int DescriptorSettingsDialog::orientHystSize()
{
    return ui->orientHystSizeSpinBox->value();
}

double DescriptorSettingsDialog::correspNNDmax()
{
    return ui->correspNNDmaxSpinBox->value();
}

double DescriptorSettingsDialog::correspLmax()
{
    return ui->correspLmaxSpinBox->value();
}

double DescriptorSettingsDialog::pyramidInitSigma()
{
    return ui->pyramidInitSigmaSpinBox->value();
}

int DescriptorSettingsDialog::pyramidLevels()
{
    return ui->pyramidLevelsSpinBox->value();
}

void DescriptorSettingsDialog::setDescRadius(int value)
{
    ui->descRadiusSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setDescNetSize(int value)
{
    ui->descNetSizeSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setDescMaskSigma(double value)
{
    ui->descMaskSigmaSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setDescHystSize(int value)
{
    ui->descHystSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setOrientRadius(int value)
{
    ui->orientRadiusSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setOrientMaskSigma(double value)
{
    ui->orientMaskSigmaSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setOrientHystSize(int value)
{
    ui->orientHystSizeSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setCorrespNNDmax(double value)
{
    ui->correspNNDmaxSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setCorrespLmax(double value)
{
    ui->correspLmaxSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setPyramidInitSigma(double value)
{
    ui->pyramidInitSigmaSpinBox->setValue(value);
}

void DescriptorSettingsDialog::setPyramidLevels(int value)
{
    ui->pyramidLevelsSpinBox->setValue(value);
}
