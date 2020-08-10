#include <pagebrakewgt.h>
#include <ui_pagebrakewgt.h>

PageBrakeWgt::PageBrakeWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageBrakeWgtClass)
{
	m_ui->setupUi(this);
	mStgs = motorStgs;
}

PageBrakeWgt::~PageBrakeWgt()
{
    delete m_ui;
}

void PageBrakeWgt::FromClassToUi()
{
	m_ui->brakeControlChk->setChecked(mStgs->brake.BrakeFlags & BRAKE_ENABLED);
	m_ui->powerOffChk->setChecked(mStgs->brake.BrakeFlags & BRAKE_ENG_PWROFF);
	m_ui->t1Spin->setValue(mStgs->brake.t1);
	m_ui->t2Spin->setValue(mStgs->brake.t2);
	m_ui->t3Spin->setValue(mStgs->brake.t3);
	m_ui->t4Spin->setValue(mStgs->brake.t4);
}

void PageBrakeWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->brake.BrakeFlags = 0;

	lmStgs->brake.t1 = m_ui->t1Spin->value();
	lmStgs->brake.t2 = m_ui->t2Spin->value();
	lmStgs->brake.t3 = m_ui->t3Spin->value();
	lmStgs->brake.t4 = m_ui->t4Spin->value();

	setUnsetBit(m_ui->brakeControlChk->isChecked(), &lmStgs->brake.BrakeFlags, BRAKE_ENABLED);
	setUnsetBit(m_ui->powerOffChk->isChecked(), &lmStgs->brake.BrakeFlags, BRAKE_ENG_PWROFF);
}
