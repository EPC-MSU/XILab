#include "pageuiconfigwgt.h"
#include "ui_pageuiconfigwgt.h"

PageUiConfigWgt::PageUiConfigWgt(QWidget *parent, AttenSettings* settings) :
    QWidget(parent),
    ui(new Ui::PageUiConfigWgt)
{
    ui->setupUi(this);
	skinSettings = settings;
    ui->GenSkinBut->setChecked(true);
    connect(ui->GenSkinBut,SIGNAL(toggled(bool)), this, SLOT(GeneralChoose(bool)));
    connect(ui->AttenSkinBut,SIGNAL(toggled(bool)),this,SLOT(AttenuatorChoose(bool)));
}

PageUiConfigWgt::~PageUiConfigWgt()
{
    delete ui;
}

void PageUiConfigWgt::FromClassToUi()
{
	if(skinSettings->skinType==GeneralSkin){
		ui->GenSkinBut->setChecked(true);
	}
	else if (skinSettings->skinType==AttenuatorSkin){
		ui->AttenSkinBut->setChecked(true);
	}
}

void PageUiConfigWgt::SetDisabledAtten(bool set)
{	
	ui->GenSkinBut->setChecked(set);
	ui->groupBox->setDisabled(set);
}

void PageUiConfigWgt::FromUiToClass()
{
	if (ui->GenSkinBut->isChecked()){
		skinSettings->skinType = GeneralSkin;
	}
	else if (ui->AttenSkinBut->isChecked()){
		skinSettings->skinType = AttenuatorSkin;
	}
}

void PageUiConfigWgt::FromUiToSettings(QSettings *settings){
	settings->beginGroup("TypeSkin");
    if (ui->GenSkinBut->isChecked()){
		settings->setValue("TypeSkin","GeneralMotor");
	}
	else if (ui->AttenSkinBut->isChecked()){
		settings->setValue("TypeSkin","Attenuator");
	}
	settings->endGroup();
}

QString PageUiConfigWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	QString errors;
	QSettings* currentSettings;
	if (settings != NULL)
		currentSettings = settings;
	else
		currentSettings = default_stgs;

	currentSettings->beginGroup("TypeSkin");
	QString typeskin = currentSettings->value("TypeSkin").toString();
	if (currentSettings->value("TypeSkin").toString() == "GeneralMotor") {
		ui->GenSkinBut->setChecked(true);
	}
	else if (currentSettings->value("TypeSkin").toString() == "Attenuator") {
		ui->AttenSkinBut->setChecked(true);
	} else
		errors+="\"TypeSkin\" key is invalid\n";
	currentSettings->endGroup();

	return errors;
}
