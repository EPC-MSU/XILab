#include <pageuserunitswgt.h>
#include <ui_pageuserunitswgt.h>
#include <float.h>
#include "functions.h"
#include <QFileDialog>

PageUserUnitsWgt::PageUserUnitsWgt(QWidget *parent, UserUnitSettings *_uuStgs, DeviceInterface *_devinterface) :
	QWidget(parent),
	m_ui(new Ui::PageUserUnitsWgtClass)
{
    m_ui->setupUi(this);
	uuStgs = _uuStgs;
	this->devface = _devinterface;

	fixGrayout(m_ui->enableChk);
#ifndef SERVICEMODE
	m_ui->pushButton->setDisabled(true);
#endif
	m_ui->stepMultiplier->setRange(1e-9, DBL_MAX);
	m_ui->stepMultiplier->setTempChangeAllowed(true);
	m_ui->stepMultiplier->setFloatStyleAllowed(false); // blocks userunits control from becoming denominated in userunits :)
	m_ui->stepMultiplier->setUnitType(UserUnitSettings::TYPE_COORD);

	m_ui->unitMultiplier->setRange(1e-9, DBL_MAX);
	m_ui->unitMultiplier->setValue(888);

	QObject::connect(m_ui->enableChk, SIGNAL(clicked(bool)), this, SIGNAL(SgnChangeUU()));
	QObject::connect(m_ui->stepMultiplier, SIGNAL(editingFinished()), this, SIGNAL(SgnChangeUU()));
	QObject::connect(m_ui->unitMultiplier, SIGNAL(editingFinished()), this, SIGNAL(SgnChangeUU()));
	QObject::connect(m_ui->precisionValue, SIGNAL(editingFinished()), this, SLOT(OnValidationUserSettings()));
	QObject::connect(m_ui->unitEdit, SIGNAL(editingFinished()), this, SIGNAL(SgnChangeUU()));


	QObject::connect(m_ui->pushButton1, SIGNAL(clicked()), this, SLOT(OnLoadTableBtnClicked()));
	QObject::connect(m_ui->pushButton2, SIGNAL(clicked()), this, SLOT(OnCloseTableBtnClicked()));
}

PageUserUnitsWgt::~PageUserUnitsWgt()
{
	delete m_ui;
}


void PageUserUnitsWgt::FromUiToSettings(QSettings *settings)
{
	settings->beginGroup("User_units");

	settings->setValue("Enable", m_ui->enableChk->isChecked());
	settings->setValue("Step_multiplier", m_ui->stepMultiplier->value());
	settings->setValue("Unit_multiplier", m_ui->unitMultiplier->value());
	settings->setValue("Precision", m_ui->precisionValue->value());
	settings->setValue("Unit", m_ui->unitEdit->text());

	settings->endGroup();
}

QString PageUserUnitsWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	QString errors;

	settings->beginGroup("User_units");
	if(default_stgs != NULL)
		default_stgs->beginGroup("User_units");

	if(settings->contains("Enable"))
		m_ui->enableChk->setChecked(settings->value("Enable", false).toBool());
	else if(default_stgs != NULL)
		m_ui->enableChk->setChecked(default_stgs->value("Enable", false).toBool());
	else 
		errors+="\"Enable\" key is not found\n";

	if(settings->contains("Step_multiplier"))
		m_ui->stepMultiplier->setValue(settings->value("Step_multiplier", 0).toDouble());
	else if(default_stgs != NULL)
		m_ui->stepMultiplier->setValue(default_stgs->value("Step_multiplier", 0).toDouble());
	else 
		errors+="\"Step_multiplier\" key is not found\n";

	if(settings->contains("Unit_multiplier"))
		m_ui->unitMultiplier->setValue(settings->value("Unit_multiplier", 0).toDouble());
	else if(default_stgs != NULL)
		m_ui->unitMultiplier->setValue(default_stgs->value("Unit_multiplier", 0).toDouble());
	else 
		errors+="\"Unit_multiplier\" key is not found\n";

	if(settings->contains("Precision"))
		m_ui->precisionValue->setValue(settings->value("Precision", 3).toUInt());
	else if(default_stgs != NULL)
		m_ui->precisionValue->setValue(default_stgs->value("Precision", 3).toUInt());
	else 
		errors+="\"Precision\" key is not found\n";

	if(settings->contains("Unit"))
		m_ui->unitEdit->setText(settings->value("Unit", "mm").toString());
	else if(default_stgs != NULL)
		m_ui->unitEdit->setText(default_stgs->value("Unit", "mm").toString());
	else 
		errors+="\"Unit\" key is not found\n";

	settings->endGroup();
	if(default_stgs != NULL)
		default_stgs->endGroup();

	return errors;
}

void PageUserUnitsWgt::FromClassToUi(bool _emit)
{
	m_ui->enableChk->setChecked(uuStgs->enable);
	m_ui->stepMultiplier->setValue(uuStgs->stepMult);
	m_ui->unitMultiplier->setValue(uuStgs->unitMult);
	m_ui->precisionValue->setValue(uuStgs->precision);
	m_ui->unitEdit->setText(uuStgs->unitName);
	if (_emit) {
		emit SgnChangeUU();
	}
}

void PageUserUnitsWgt::SetChecked(bool _emit, bool enable)
{
	m_ui->enableChk->setChecked(enable);
	m_ui->stepMultiplier->setValue(uuStgs->stepMult);
	m_ui->unitMultiplier->setValue(uuStgs->unitMult);
	m_ui->precisionValue->setValue(uuStgs->precision);
	m_ui->unitEdit->setText(uuStgs->unitName);
	if (_emit) {
		emit SgnChangeUU();
	}
}

void PageUserUnitsWgt::FromUiToClass()
{
	uuStgs->enable = m_ui->enableChk->isChecked();
	uuStgs->stepMult = m_ui->stepMultiplier->value();
	uuStgs->unitMult = m_ui->unitMultiplier->value();
	uuStgs->precision = m_ui->precisionValue->value();
	uuStgs->unitName = m_ui->unitEdit->text();
}

void PageUserUnitsWgt::OnLoadTableBtnClicked()
{
	QString restoreErrors;
	QString filename, load_path;

	load_path = getDefaultProfilesPath();

	filename = QFileDialog::getOpenFileName(this, tr("Open table file"), load_path, tr("Table files (*.tbl);;All files (*.*)"));
	
	if (!filename.isEmpty())
	{		
		char namefile[255];
		int i;
		for (i = 0; i<filename.length(); i++){
			namefile[i] = (char)filename[i].toAscii();
		}
		namefile[i] = '\0';

		result = this->devface->load_calibration_table(namefile);
		if (result == result_ok) m_ui->namefileLbl->setText(namefile);
	}
}

void PageUserUnitsWgt::OnCloseTableBtnClicked()
{

	result = this->devface->load_calibration_table(NULL);
	if (result == result_ok) m_ui->namefileLbl->setText("\"\"");

}

void PageUserUnitsWgt::OnValidationUserSettings()
{
	if (m_ui->precisionValue->value() > MAX_PRECISION)
		m_ui->precisionValue->setValue(MAX_PRECISION);
	emit SgnChangeUU();
}
