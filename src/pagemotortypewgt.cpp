#include <pagemotortypewgt.h>
#include <ui_pagemotortypewgt.h>

PageMotorTypeWgt::PageMotorTypeWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageMotorTypeWgtClass)
{
    m_ui->setupUi(this);
	mStgs = motorStgs;

	QObject::connect(m_ui->dcMotorBtn, SIGNAL(toggled(bool)), this, SLOT(setMotorTypeDC(bool)));
	QObject::connect(m_ui->stepperMotorBtn, SIGNAL(toggled(bool)), this, SLOT(setMotorTypeStep(bool)));
	QObject::connect(m_ui->bldcMotorBtn, SIGNAL(toggled(bool)), this, SLOT(setMotorTypeBLDC(bool)));

	//пока не используется
	m_ui->unknownMotorBtn->setVisible(false);
	m_ui->unknownDriverBtn->setVisible(false);;

#ifndef SERVICEMODE
	m_ui->discreteDriverBtn->setDisabled(true);
#endif

	m_ui->dcMotorBtn->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->stepperMotorBtn->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->bldcMotorBtn->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->unknownMotorBtn->setStyleSheet("QRadioButton { color : #B0A000; }");

	m_ui->discreteDriverBtn->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->integrateDriverBtn->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->extDriverBtn->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->unknownDriverBtn->setStyleSheet("QRadioButton { color : #B0A000; }");
}

PageMotorTypeWgt::~PageMotorTypeWgt()
{
    delete m_ui;
}

unsigned int PageMotorTypeWgt::getCurrentMotorType()
{
	if(m_ui->stepperMotorBtn->isChecked())
		return ENGINE_TYPE_STEP;
	else if(m_ui->dcMotorBtn->isChecked())
		return ENGINE_TYPE_DC;
	else if(m_ui->bldcMotorBtn->isChecked())
		return ENGINE_TYPE_BRUSHLESS;
	else
		return ENGINE_TYPE_NONE;
}

void PageMotorTypeWgt::setMotorTypeDC(bool checked)
{
	if (checked)
		emit SgnMotorTypeChanged(ENGINE_TYPE_DC);
}

void PageMotorTypeWgt::setMotorTypeStep(bool checked)
{
	if (checked)
		emit SgnMotorTypeChanged(ENGINE_TYPE_STEP);
}

void PageMotorTypeWgt::setMotorTypeBLDC(bool checked)
{
	if (checked)
		emit SgnMotorTypeChanged(ENGINE_TYPE_BRUSHLESS);
}

void PageMotorTypeWgt::FromClassToUi()
{
	switch(mStgs->entype.EngineType)
	{
		case ENGINE_TYPE_DC:
			m_ui->dcMotorBtn->setChecked(true);
			break;
		case ENGINE_TYPE_STEP:
			m_ui->stepperMotorBtn->setChecked(true);
			break;
		case ENGINE_TYPE_BRUSHLESS:
			m_ui->bldcMotorBtn->setChecked(true);
			break;
		default:
			m_ui->unknownMotorBtn->setChecked(true);
			break;
	}

	switch(mStgs->entype.DriverType)
	{
		case DRIVER_TYPE_DISCRETE_FET:
			m_ui->discreteDriverBtn->setChecked(true);
			break;
		case DRIVER_TYPE_INTEGRATE:
			m_ui->integrateDriverBtn->setChecked(true);
			break;
		case DRIVER_TYPE_EXTERNAL:
			m_ui->extDriverBtn->setChecked(true);
			break;
		default:
			m_ui->unknownDriverBtn->setChecked(true);
			break;
	}
}

void PageMotorTypeWgt::FromUiToClass(MotorSettings *lmStgs)
{
	if(m_ui->dcMotorBtn->isChecked())
		lmStgs->entype.EngineType = ENGINE_TYPE_DC;
	else if(m_ui->bldcMotorBtn->isChecked())
		lmStgs->entype.EngineType = ENGINE_TYPE_BRUSHLESS;
	else if(m_ui->stepperMotorBtn->isChecked())
		lmStgs->entype.EngineType = ENGINE_TYPE_STEP;

	if(m_ui->discreteDriverBtn->isChecked())
		lmStgs->entype.DriverType = DRIVER_TYPE_DISCRETE_FET;
	else if(m_ui->integrateDriverBtn->isChecked())
		lmStgs->entype.DriverType = DRIVER_TYPE_INTEGRATE;
	else if(m_ui->extDriverBtn->isChecked())
		lmStgs->entype.DriverType = DRIVER_TYPE_EXTERNAL;
}

void PageMotorTypeWgt::disableBLDC(bool flag)
{
	this->m_ui->bldcMotorBtn->setEnabled(!flag);
}