#include <pageslidersetupwgt.h>
#include <ui_pageslidersetupwgt.h>
#include <functions.h>
#include <xsettings.h>

PageSliderSetupWgt::PageSliderSetupWgt(QWidget *parent, SliderSettings *sliderStgs) :
	QWidget(parent),
	m_ui(new Ui::PageSliderSetupWgtClass)
{
	sStgs = sliderStgs;
    m_ui->setupUi(this);

	m_ui->leftPointValue->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->leftPointValue->setFloatStyleAllowed(true);
	m_ui->leftPointValue->setTempChangeAllowed(true);

	m_ui->rightPointValue->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->rightPointValue->setFloatStyleAllowed(true);
	m_ui->rightPointValue->setTempChangeAllowed(true);
}

PageSliderSetupWgt::~PageSliderSetupWgt()
{
	delete m_ui;
}

void PageSliderSetupWgt::FromUiToSettings(QSettings *settings)
{
	settings->beginGroup("GeneralMotor_setup");
	settings->setValue("Left_point", m_ui->leftPointValue->basevalueInt());
	settings->setValue("Right_point", m_ui->rightPointValue->basevalueInt());
	settings->setValue("LoadFromDevice_enable", m_ui->LoadFromDeviceChk->isChecked());
	settings->setValue("WatchOverLimits_enable", m_ui->WatchOverLimitsChk->isChecked());
	settings->setValue("ShowSecondaryPosition_enable", m_ui->showSecondaryChk->isChecked());
	settings->endGroup();

}

QString PageSliderSetupWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	QString errors;

	settings->beginGroup("GeneralMotor_setup");

	if(default_stgs != NULL)
		default_stgs->beginGroup("GeneralMotor_setup");

	if(settings->contains("Left_point"))
		m_ui->leftPointValue->setBaseValue(settings->value("Left_point", 0).toDouble());
	else if(default_stgs != NULL)
		m_ui->leftPointValue->setBaseValue(default_stgs->value("Left_point", 0).toDouble());
	else 
		errors+="\"Left_point\" key is not found\n";

	if(settings->contains("Right_point"))
		m_ui->rightPointValue->setBaseValue(settings->value("Right_point", 0).toDouble());
	else if(default_stgs != NULL)
		m_ui->rightPointValue->setBaseValue(default_stgs->value("Right_point", 0).toDouble());
	else 
		errors+="\"Right_point\" key is not found\n";

	if(settings->contains("LoadFromDevice_enable"))
		m_ui->LoadFromDeviceChk->setChecked(settings->value("LoadFromDevice_enable", true).toBool());
	else if(default_stgs != NULL)
		m_ui->LoadFromDeviceChk->setChecked(default_stgs->value("LoadFromDevice_enable", true).toBool());
	else 
		errors+="\"LoadFromDevice_enable\" key is not found\n";

	if(settings->contains("WatchOverLimits_enable"))
		m_ui->WatchOverLimitsChk->setChecked(settings->value("WatchOverLimits_enable", true).toBool());
	else if(default_stgs != NULL)
		m_ui->WatchOverLimitsChk->setChecked(default_stgs->value("WatchOverLimits_enable", true).toBool());
	else 
		errors+="\"WatchOverLimits_enable\" key is not found\n";

	if(settings->contains("ShowSecondaryPosition_enable"))
		m_ui->showSecondaryChk->setChecked(settings->value("ShowSecondaryPosition_enable", true).toBool());
	else if(default_stgs != NULL)
		m_ui->showSecondaryChk->setChecked(default_stgs->value("ShowSecondaryPosition_enable", true).toBool());
	else 
		errors+="\"ShowSecondaryPosition_enable\" key is not found\n";

	settings->endGroup();
	if(default_stgs != NULL)
		default_stgs->endGroup();
	return errors;
}

void PageSliderSetupWgt::FromClassToUi()
{
	m_ui->leftPointValue->setBaseValue(sStgs->left);
	m_ui->rightPointValue->setBaseValue(sStgs->right);
	m_ui->LoadFromDeviceChk->setChecked(sStgs->loadfromdevice);
	m_ui->WatchOverLimitsChk->setChecked(sStgs->watchoverlimits);
	m_ui->showSecondaryChk->setChecked(sStgs->showsecondary);
}

void PageSliderSetupWgt::FromUiToClass()
{
	sStgs->left = m_ui->leftPointValue->basevalue();
	sStgs->right = m_ui->rightPointValue->basevalue();
	sStgs->loadfromdevice = m_ui->LoadFromDeviceChk->isChecked();
	sStgs->watchoverlimits = m_ui->WatchOverLimitsChk->isChecked();
	sStgs->showsecondary = m_ui->showSecondaryChk->isChecked();
}

void PageSliderSetupWgt::SetDisabledPart(bool set)
{
	m_ui->groupBox_2->setDisabled(set);
	m_ui->WatchOverLimitsChk->setDisabled(set);
}
