#include <pagegraphcommonwgt.h>
#include <ui_pagegraphcommonwgt.h>

PageGraphCommonWgt::PageGraphCommonWgt(QWidget *parent, GraphCommonSettings *gcommonStgs) :
	QWidget(parent),
	m_ui(new Ui::PageGraphCommonWgtClass)
{
	gcomStgs = gcommonStgs;
    m_ui->setupUi(this);
}

PageGraphCommonWgt::~PageGraphCommonWgt()
{
	delete m_ui;
}

void PageGraphCommonWgt::FromUiToSettings(QSettings *settings)
{
	settings->beginGroup("Graph_common_setup");

	settings->setValue("Datapoints", m_ui->datapointsValue->value());
	settings->setValue("Update_interval", m_ui->intervalValue->value());
	settings->setValue("dataUpdate_enable", m_ui->dataUpdateChk->isChecked());
	settings->setValue("Autostart_on_open", m_ui->autostartOnOpen->isChecked());

	settings->endGroup();
}

QString PageGraphCommonWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	QString errors;

	settings->beginGroup("Graph_common_setup");
	if(default_stgs != NULL)
		default_stgs->beginGroup("Graph_common_setup");

	if(settings->contains("Datapoints"))
		m_ui->datapointsValue->setValue(settings->value("Datapoints", 0).toUInt());
	else if(default_stgs != NULL)
		m_ui->datapointsValue->setValue(default_stgs->value("Datapoints", 0).toUInt());
	else
		errors+="\"Datapoints\" key is not found\n";


	if(settings->contains("Update_interval"))
		m_ui->intervalValue->setValue(settings->value("Update_interval", 0).toInt());
	else if(default_stgs != NULL)
		m_ui->intervalValue->setValue(default_stgs->value("Update_interval", 0).toInt());
	else 
		errors+="\"Update_interval\" key is not found\n";

	if(settings->contains("dataUpdate_enable"))
		m_ui->dataUpdateChk->setChecked(settings->value("dataUpdate_enable", true).toBool());
	else if(default_stgs != NULL)
		m_ui->dataUpdateChk->setChecked(default_stgs->value("dataUpdate_enable", true).toBool());
	else 
		errors+="\"dataUpdate_enable\" key is not found\n";

	if(settings->contains("Autostart_on_open"))
		m_ui->autostartOnOpen->setChecked(settings->value("Autostart_on_open", true).toBool());
	else if(default_stgs != NULL)
		m_ui->autostartOnOpen->setChecked(default_stgs->value("Autostart_on_open", true).toBool());
	else 
		errors+="\"Autostart_on_open\" key is not found\n";

	settings->endGroup();
	if(default_stgs != NULL)
		default_stgs->endGroup();

	return errors;
}

void PageGraphCommonWgt::FromClassToUi()
{
	m_ui->datapointsValue->setValue(gcomStgs->datapoints);
	m_ui->intervalValue->setValue(gcomStgs->interval);
	m_ui->dataUpdateChk->setChecked(gcomStgs->dataUpdate);
	m_ui->autostartOnOpen->setChecked(gcomStgs->autostart);
}

void PageGraphCommonWgt::FromUiToClass()
{
	gcomStgs->datapoints = m_ui->datapointsValue->value();
	gcomStgs->interval = m_ui->intervalValue->value();
	gcomStgs->dataUpdate = m_ui->dataUpdateChk->isChecked();
	gcomStgs->autostart = m_ui->autostartOnOpen->isChecked();
}