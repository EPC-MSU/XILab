#include <pagegraphwgt.h>
#include <ui_pagegraphwgt.h>
#include <pagegraphsetupwgt.h>

PageGraphWgt::PageGraphWgt(QWidget *parent, GraphCurveSettings *_gcStgs, QString _group) :
	QWidget(parent),
	m_ui(new Ui::PageGraphWgtClass)
{
	gcStgs = _gcStgs;
	group = _group;
	m_ui->setupUi(this);

	m_ui->groupBox->setDisabled(m_ui->autoscaleChk->isChecked());
	CurveInitUI(m_ui->linestyleBox);
}

PageGraphWgt::~PageGraphWgt()
{
	delete m_ui;
}

void PageGraphWgt::FromUiToSettings(QSettings *settings)
{
	CurveGetSettings(settings, group,
				m_ui->linewidthValue,
				m_ui->minScaleValue,
				m_ui->maxScaleValue,
				m_ui->antialiasingChk,
				m_ui->autoscaleChk,
				m_ui->colorBox,
				m_ui->linestyleBox);
}

QString PageGraphWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	return CurveSetSettings(group,
				m_ui->linewidthValue,
				m_ui->minScaleValue,
				m_ui->maxScaleValue,
				m_ui->antialiasingChk,
				m_ui->autoscaleChk,
				m_ui->colorBox,
				m_ui->linestyleBox,
				settings, 
				default_stgs);
}

void PageGraphWgt::FromClassToUi()
{
	CurveUpdatePage(gcStgs,
				m_ui->linewidthValue,
				m_ui->minScaleValue,
				m_ui->maxScaleValue,
				m_ui->antialiasingChk,
				m_ui->autoscaleChk,
				m_ui->colorBox,
				m_ui->linestyleBox);
}

void PageGraphWgt::FromUiToClass()
{
	CurveGetData(gcStgs,
				 m_ui->linewidthValue,
				 m_ui->minScaleValue,
				 m_ui->maxScaleValue,
				 m_ui->antialiasingChk,
				 m_ui->autoscaleChk,
				 m_ui->colorBox,
				 m_ui->linestyleBox);
}

void PageGraphWgt::SetCaption(QString caption)
{
	m_ui->mainBox->setTitle(caption);
}

void PageGraphWgt::HideColor()
{
	m_ui->label->hide();
	m_ui->colorBox->hide();
}