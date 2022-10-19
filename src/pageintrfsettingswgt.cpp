#include "pageintrfsettingswgt.h"
#include "ui_pageintrfsettingswgt.h"
#include <xsettings.h>
#include <functions.h>

PageIntrfSettingsWgt::PageIntrfSettingsWgt(QWidget *parent, AttenSettings *_intStgs) :
    QWidget(parent),
    ui(new Ui::PageIntrfSettingsWgt)
{
    intStgs = _intStgs;
    ui->setupUi(this);
    for(int j =0;j !=2;j++){
        vboxHTrans[j] = new QHBoxLayout();
        titleBox = new QVBoxLayout();
        titleLabel[0] = new QLabel("Filter");
        titleLabel[1] = new QLabel("Transmittance");
        titleBox->addWidget(titleLabel[0],0,Qt::AlignLeft);
        titleBox->addWidget(titleLabel[1]);
        vboxHTrans[j]->addLayout(titleBox);
        for(int i =0;i != FILTER_COUNT;i++){
            (!j) ? trans_line[i+FILTER_COUNT*j] = new QLineEdit(QString::number(intStgs->wheel1Tran[i])) : trans_line[i+FILTER_COUNT*j] = new QLineEdit(QString::number(intStgs->wheel2Tran[i]));
            vboxVTrans[i+FILTER_COUNT*j] = new QVBoxLayout();
            trans_label[i+FILTER_COUNT*j] = new QLabel(QString::number(i+FILTER_COUNT*j+1));
            vboxVTrans[i+FILTER_COUNT*j]->addWidget(trans_label[i+FILTER_COUNT*j],0,Qt::AlignLeft);
            vboxVTrans[i+FILTER_COUNT*j]->addWidget( trans_line[i+FILTER_COUNT*j]);
            vboxHTrans[j]->addLayout(vboxVTrans[i+FILTER_COUNT*j]);
        }
    }

    ui->Wheel1Box->setLayout(vboxHTrans[0]);
    ui->Wheel2Box->setLayout(vboxHTrans[1]);
    wavelength_group = new QButtonGroup();
    wavelength_group->addButton(ui->wave1but);
    wavelength_group->addButton(ui->wave2but);
    wavelength_group->addButton(ui->wave3but);
    wavelength_group->setExclusive(true);
    ui->wave1but->setChecked(true);

    wheels_group = new QButtonGroup();
    wheels_group->addButton(ui->rBoneWheel);
    wheels_group->addButton(ui->rBtwoWheel);
    wheels_group->setExclusive(true);
    ui->rBtwoWheel->setChecked(true);

    signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(ui->rBoneWheel,one_wheel);//use one wheel
    signalMapper->setMapping(ui->rBtwoWheel,two_wheels);//use two wheels

    connect(ui->rBoneWheel,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(ui->rBtwoWheel,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(signalMapper, SIGNAL(mapped(int)),this,SLOT(recClickedInterface(int)));
    connect(signalMapper, SIGNAL(mapped(int)),this,SLOT(setDisabled(int)));
    //hide not functional buttons
    ui->wave1but->hide();
    ui->wave2but->hide();
    ui->wave3but->hide();
    //hide not functional wavelength line
    ui->lineWav->hide();
    ui->scale_lab->hide();
}

void PageIntrfSettingsWgt::setDisabled(int NumInt){
    switch(NumInt){
    //use one wheel
    case one_wheel:
        ui->Wheel2Box->setDisabled(true);
        break;
    //use two wheels
    case two_wheels:
        ui->Wheel2Box->setEnabled(true);
        break;

    }

}

void PageIntrfSettingsWgt::recClickedInterface(int NumInt){
    emit clickedInterface(NumInt,intStgs);

}

PageIntrfSettingsWgt::~PageIntrfSettingsWgt()
{
    delete ui;
    delete titleBox;
    for (int i =0;i != WHEEL_COUNT*FILTER_COUNT;i++) { delete trans_line[i]; delete trans_label[i]; delete vboxVTrans[i];}
    for (int i =0;i != WHEEL_COUNT;i++) { delete vboxHTrans[i]; delete titleLabel[i];}
    delete signalMapper;
}


void PageIntrfSettingsWgt::FromUiToSettings(QSettings *settings)
{
    settings->beginGroup("Attenuator_setup");

    QString Num_of_Wheels = "number_of_wheels";

    if(ui->rBoneWheel->isChecked()){
        settings->setValue(Num_of_Wheels,"One");
    }
    else if (ui->rBtwoWheel->isChecked()) {
        settings->setValue(Num_of_Wheels,"Two");
    }

    for (int i =0; i != WHEEL_COUNT*FILTER_COUNT; i++){
        int numWheel  = 0;
        (i < FILTER_COUNT) ? numWheel = 1 : numWheel = 2;
        QString valueTransmTitle = "Wheel_" + QString::number(numWheel)+
                                   "Transmittance_"+ QString::number(i+1);
        settings->setValue(valueTransmTitle,trans_line[i]->text());
    }
    settings->endGroup();
}

QString PageIntrfSettingsWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
    QString errors;

    QSettings* currentSettings = settings;

    (currentSettings != NULL) ? currentSettings=settings : currentSettings=default_stgs;

	currentSettings->beginGroup("Attenuator_setup");
	if (currentSettings->value("number_of_wheels").toString() == "One") {
		ui->rBoneWheel->setChecked(true);
		ui->Wheel2Box->setDisabled(true);
	}
	else if (currentSettings->value("number_of_wheels").toString() == "Two"){
		ui->rBtwoWheel->setChecked(true);
		ui->Wheel2Box->setEnabled(true);
	}

    for (int i = 0;i != FILTER_COUNT; i++) {
        QString valueTransmTitle1 = "Wheel_1Transmittance_" + QString::number(i+1);
        QString valueTransmTitle2 = "Wheel_2Transmittance_" + QString::number(i+FILTER_COUNT+1);
        trans_line[i]->setText(currentSettings->value(valueTransmTitle1).toString());
        trans_line[i+FILTER_COUNT]->setText(currentSettings->value(valueTransmTitle2).toString());

    }
  
    currentSettings->endGroup();

    return errors;
}


void PageIntrfSettingsWgt::FromClassToUi()
{
    for (int i = 0;i != FILTER_COUNT; i++) {
        trans_line[i]->setText(QString::number(intStgs->wheel1Tran[i]));
        trans_line[i+FILTER_COUNT]->setText(QString::number(intStgs->wheel2Tran[i]));
    }
	(intStgs->twoWheels) ? ui->rBtwoWheel->setChecked(true) : ui->rBoneWheel->setChecked(true);
	ui->Wheel2Box->setEnabled(intStgs->twoWheels);
}

void PageIntrfSettingsWgt::FromUiToClass()
{
//	intStgs->selected_wavelength = ?
//	intStgs->selected_wavelength_value = ?
//	intStgs->skinType = ???  (your setting is in another castle: see pagetypeskin.cpp)
    for (int i = 0;i != FILTER_COUNT; i++) {
        intStgs->wheel1Tran[i] = trans_line[i]->text().toDouble();
        intStgs->wheel2Tran[i] = trans_line[i+FILTER_COUNT]->text().toDouble();
    }
	intStgs->twoWheels = ui->rBtwoWheel->isChecked();
}
