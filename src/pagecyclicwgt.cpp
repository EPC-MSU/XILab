#include <pagecyclicwgt.h>
#include <ui_pagecyclicwgt.h>

PageCyclicWgt::PageCyclicWgt(QWidget *parent, CyclicSettings *cyclicStgs) :
	QWidget(parent),
	m_ui(new Ui::PageCyclicWgtClass)
{
    m_ui->setupUi(this);
	cStgs = cyclicStgs;

	m_ui->leftPointValue->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->leftPointValue->setFloatStyleAllowed(true);
	m_ui->leftPointValue->setTempChangeAllowed(true);

	m_ui->rightPointValue->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->rightPointValue->setFloatStyleAllowed(true);
	m_ui->rightPointValue->setTempChangeAllowed(true);
}

PageCyclicWgt::~PageCyclicWgt()
{
	delete m_ui;
}

void PageCyclicWgt::FromUiToSettings(QSettings *settings)
{
	settings->beginGroup("Cyclic");

	settings->setValue("Left_point", m_ui->leftPointValue->basevalueInt());
	settings->setValue("Right_point", m_ui->rightPointValue->basevalueInt());
	
	QString cyclic_type;
	if(m_ui->btbBtn->isChecked()) cyclic_type = "border_to_border";
	else if (m_ui->ptpFineBtn->isChecked()) cyclic_type = "ptp_fine";
	else cyclic_type = "undefined";

	settings->setValue("Cyclic_type", cyclic_type);

	settings->endGroup();
}

QString PageCyclicWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	QString errors;

	settings->beginGroup("Cyclic");
	if(default_stgs != NULL)
		default_stgs->beginGroup("Cyclic");

	if(settings->contains("Left_point")){
		m_ui->leftPointValue->setBaseValue(settings->value("Left_point", 0).toDouble());
	}
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

	if(settings->contains("Cyclic_type")){
		if(settings->value("Cyclic_type", "border_to_border").toString() == "border_to_border")
			m_ui->btbBtn->setChecked(true);
		else if(settings->value("Cyclic_type", "border_to_border").toString() == "ptp_fine")
			m_ui->ptpFineBtn->setChecked(true);
		else
			errors+="\"Cyclic_type\" value is wrong\n";
	}
	else if(default_stgs != NULL){
		if(default_stgs->value("Cyclic_type", "border_to_border").toString() == "border_to_border")
			m_ui->btbBtn->setChecked(true);
		else if(default_stgs->value("Cyclic_type", "border_to_border").toString() == "ptp_fine")
			m_ui->ptpFineBtn->setChecked(true);
		else
			errors+="\"Cyclic_type\" value is wrong\n";
	}
	else
		errors+="\"Cyclic_type\" key is not found\n";
	
	settings->endGroup();
	if(default_stgs != NULL)
		default_stgs->endGroup();

	return errors;
}

void PageCyclicWgt::FromClassToUi()
{
	switch(cStgs->GetType())
	{
		case CyclicSettings::UnitType::CYCLICUNKNOWN:
			break;
		case CyclicSettings::UnitType::CYCLICBTB:
			m_ui->btbBtn->setChecked(true);
			break;
		case CyclicSettings::UnitType::CYCLICPTP_FINE:
			m_ui->ptpFineBtn->setChecked(true);
			break;
	}

	m_ui->leftPointValue->setBaseValue(cStgs->GetLeft());
	m_ui->rightPointValue->setBaseValue(cStgs->GetRight());
}

void PageCyclicWgt::FromUiToClass()
{
	cStgs->SetType(GetCyclicType());
	cStgs->SetLeft(m_ui->leftPointValue->basevalue());
	cStgs->SetRight(m_ui->rightPointValue->basevalue());
}

CyclicSettings::UnitType PageCyclicWgt::GetCyclicType()
{
	if (m_ui->btbBtn->isChecked())
		return CyclicSettings::UnitType::CYCLICBTB;
	if (m_ui->ptpFineBtn->isChecked())
		return CyclicSettings::UnitType::CYCLICPTP_FINE;
	return CyclicSettings::UnitType::CYCLICUNKNOWN;
}
