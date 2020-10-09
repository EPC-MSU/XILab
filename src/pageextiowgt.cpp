#include <pageextiowgt.h>
#include <ui_pageextiowgt.h>

PageExtioWgt::PageExtioWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageExtioWgtClass)
{
	m_ui->setupUi(this);
	mStgs = motorStgs;

	m_ui->checkBox->toggle();
}

PageExtioWgt::~PageExtioWgt()
{
    delete m_ui;
}

void PageExtioWgt::FromClassToUi()
{
	m_ui->checkBox->setChecked(mStgs->extio.EXTIOSetupFlags & EXTIO_SETUP_OUTPUT);
	m_ui->checkBox_2->setChecked(mStgs->extio.EXTIOSetupFlags & EXTIO_SETUP_INVERT);

	m_ui->comboBox->setCurrentIndex(mStgs->extio.EXTIOModeFlags & 0xF);
	m_ui->comboBox_2->setCurrentIndex(mStgs->extio.EXTIOModeFlags >> 4);
}

void PageExtioWgt::FromUiToClass(MotorSettings *lmStgs)
{

	lmStgs->extio.EXTIOModeFlags = 0;
	lmStgs->extio.EXTIOSetupFlags = 0;

	setUnsetBit(m_ui->checkBox->isChecked(), &lmStgs->extio.EXTIOSetupFlags, EXTIO_SETUP_OUTPUT); 
	setUnsetBit(m_ui->checkBox_2->isChecked(), &lmStgs->extio.EXTIOSetupFlags, EXTIO_SETUP_INVERT); 

	switch (m_ui->comboBox->currentIndex()) {
		case 0: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_IN_NOP; break;
		case 1: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_IN_STOP; break;
		case 2: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_IN_PWOF; break;
		case 3: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_IN_MOVR; break;
		case 4: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_IN_HOME; break;
		case 5: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_IN_ALARM; break;
		default: break;
	}
	switch (m_ui->comboBox_2->currentIndex()) {
		case 0: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_OUT_OFF; break;
		case 1: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_OUT_ON; break;
		case 2: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_OUT_MOVING; break;
		case 3: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_OUT_ALARM; break;
		case 4: lmStgs->extio.EXTIOModeFlags |= EXTIO_SETUP_MODE_OUT_MOTOR_ON; break;
		default: break;
	}

}
