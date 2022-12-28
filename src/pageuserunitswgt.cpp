#include <pageuserunitswgt.h>
#include <ui_pageuserunitswgt.h>
#include <float.h>
#include "functions.h"
#include <QFileDialog>
#include <QMessageBox>

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

	m_ui->unitMultiplier->setRange(1e-9, DBL_MAX);//1e-9
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
	if (m_ui->namefileLbl->text() != "\"\"")
		settings->setValue("Correction_table", m_ui->namefileLbl->text());
	else
		settings->setValue("Correction_table", "");
	settings->endGroup();
}

bool PageUserUnitsWgt::enableChkisChecked()
{
	return m_ui->enableChk->isChecked();
}

QString PageUserUnitsWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	QString errors;

	settings->beginGroup("User_units");
	if(default_stgs != NULL)
		default_stgs->beginGroup("User_units");

	if (settings->value("Unit", "mm").toString() != "n/a")
	{
		m_ui->enableChk->setEnabled(true);
		if (settings->contains("Enable"))
			m_ui->enableChk->setChecked(settings->value("Enable", false).toBool());
		else if (default_stgs != NULL)
			m_ui->enableChk->setChecked(default_stgs->value("Enable", false).toBool());
		else
			errors += "\"Enable\" key is not found\n";
	}
	else
		m_ui->enableChk->setDisabled(true);

	int chekValUU = 0;
	static int stateUU = 0;
	if(settings->contains("Step_multiplier"))
		if(settings->value("Step_multiplier", 1).toDouble() >= 1)
			m_ui->stepMultiplier->setValue(settings->value("Step_multiplier", 1).toDouble());
		else {
			if (m_ui->enableChk->isChecked()) {
				m_ui->enableChk->setChecked(false);
				chekValUU = 1;
			}
			m_ui->stepMultiplier->setValue(1.0);		
		}
	else if(default_stgs != NULL)
		m_ui->stepMultiplier->setValue(default_stgs->value("Step_multiplier", 1).toDouble());
	else 
		errors+="\"Step_multiplier\" key is not found\n";

	if(settings->contains("Unit_multiplier"))
		if(settings->value("Unit_multiplier", 1).toDouble() >= 1)
			m_ui->unitMultiplier->setValue(settings->value("Unit_multiplier", 1).toDouble());
		else {
			if (m_ui->enableChk->isChecked()) {
				m_ui->enableChk->setChecked(false);
				chekValUU = 1;
			}
			m_ui->unitMultiplier->setValue(1.0);
		}
	else if(default_stgs != NULL)
		m_ui->unitMultiplier->setValue(default_stgs->value("Unit_multiplier", 1).toDouble());
	else 
		errors+="\"Unit_multiplier\" key is not found\n";

	if (chekValUU && stateUU) {
		m_ui->enableChk->setChecked(false);
		QMessageBox msgBox;
		msgBox.setText("Warning");
		msgBox.setInformativeText("The settings of the user units in the profile are incorrect. They were turned off on the User unit page to prevent data corruption.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		chekValUU = 0;
		stateUU = 0;
	}
	else
		stateUU = 1;

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

	if (settings->contains("Correction_table"))
	{
		m_ui->namefileLbl->setText(settings->value("Correction_table", "\"\"").toString());

		char namefile[255];
		int i;
		for (i = 0; i<m_ui->namefileLbl->text().length(); i++){
			namefile[i] = (char)m_ui->namefileLbl->text()[i].toAscii();
		}
		namefile[i] = '\0';
		
		if (i > 1)
		{
			result = this->devface->load_calibration_table(namefile);
			if (result == result_ok) m_ui->namefileLbl->setText(namefile);
			else
			{
				uuStgs->messageType = 3; // Warning
				uuStgs->messageText = "The correction table " + m_ui->namefileLbl->text() + " specified in the profile could not be loaded.";

				m_ui->namefileLbl->setText("\"\"");
				OnCloseTableBtnClicked();
			}
		}
		else
		{
			m_ui->namefileLbl->setText("\"\"");
			OnCloseTableBtnClicked();
		}
	}
	else
	{
		if ((m_ui->namefileLbl->text() != "\"\""))
		{
			uuStgs->messageType = 1; //Qestion
			uuStgs->messageText = "Reset the correction table " + m_ui->namefileLbl->text() + " after loading the profile?";
		}
	}

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
	if (m_ui->namefileLbl->text() != uuStgs->correctionTable)
	{
		m_ui->namefileLbl->setText(uuStgs->correctionTable);
		if (uuStgs->correctionTable == "\"\"")
		{
			result = this->devface->load_calibration_table(NULL);
		}
		else
		{
			char namefile[255];
			int i;
			for (i = 0; i<uuStgs->correctionTable.length(); i++){
				namefile[i] = (char) uuStgs->correctionTable[i].toAscii();
			}
			namefile[i] = '\0';

			result = this->devface->load_calibration_table(namefile);
			if (result == result_ok) m_ui->namefileLbl->setText(namefile);
			else m_ui->namefileLbl->setText("\"\"");
		}
	}

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
	m_ui->namefileLbl->setText(uuStgs->correctionTable);
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
	if (uuStgs->correctionTable != m_ui->namefileLbl->text())
	{
		uuStgs->correctionTable = m_ui->namefileLbl->text();
	}
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
		else m_ui->namefileLbl->setText("\"\"");
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
