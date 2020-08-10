#include <pagepositionernamewgt.h>
#include <ui_pagepositionernamewgt.h>

PagePositionerNameWgt::PagePositionerNameWgt(QWidget *parent, StageSettings *_stageStgs) :
    QWidget(parent),
    m_ui(new Ui::PagePositionerNameWgtClass)
{
    m_ui->setupUi(this);
	stageStgs = _stageStgs;

}

PagePositionerNameWgt::~PagePositionerNameWgt()
{
    delete m_ui;
}

void PagePositionerNameWgt::FromClassToUi()
{
	m_ui->positionerName->setText(QString::fromUtf8(stageStgs->stage_name.PositionerName));
}

void PagePositionerNameWgt::FromUiToClass(StageSettings *sStgs)
{
	strcpy(sStgs->stage_name.PositionerName, m_ui->positionerName->text().toUtf8().data());
}

void PagePositionerNameWgt::SetDisabled(bool set)
{
	m_ui->positionerName->setDisabled(set);
}