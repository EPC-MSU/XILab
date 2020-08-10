#include <pagehomepositionwgt.h>
#include <ui_pagehomepositionwgt.h>
#include <functions.h>
#include <main.h>

PageHomePositionWgt::PageHomePositionWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageHomePositionWgtClass)
{
    m_ui->setupUi(this);

	mStgs = motorStgs;

	m_ui->speedFastBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->speedSlowBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->deltaBox->setUnitType(UserUnitSettings::TYPE_COORD);
}

PageHomePositionWgt::~PageHomePositionWgt()
{
    delete m_ui;
}

void PageHomePositionWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->home.FastHome = m_ui->speedFastBox->UgetBaseStep();
	lmStgs->home.uFastHome = m_ui->speedFastBox->UgetBaseUStep();
	lmStgs->home.SlowHome = m_ui->speedSlowBox->UgetBaseStep();
	lmStgs->home.uSlowHome = m_ui->speedSlowBox->UgetBaseUStep();
	lmStgs->home.HomeDelta =  m_ui->deltaBox->getBaseStep();
	lmStgs->home.uHomeDelta = m_ui->deltaBox->getBaseUStep();

	lmStgs->home.HomeFlags = 0;
	if(m_ui->dirFastBox->currentIndex() == 1)
		lmStgs->home.HomeFlags |= HOME_DIR_FIRST;
	if(m_ui->dirBox->currentIndex() == 1)
		lmStgs->home.HomeFlags |= HOME_DIR_SECOND;
	if(m_ui->fromEndChk->isChecked())
		lmStgs->home.HomeFlags |= HOME_MV_SEC_EN;
	if(m_ui->halfRevChk->isChecked())
		lmStgs->home.HomeFlags |= HOME_HALF_MV;

	if(m_ui->StopCondBox->currentIndex() == 0)
		lmStgs->home.HomeFlags |= HOME_STOP_FIRST_REV;
	if(m_ui->StopCondBox->currentIndex() == 1)
		lmStgs->home.HomeFlags |= HOME_STOP_FIRST_SYN;
	if(m_ui->StopCondBox->currentIndex() == 2)
		lmStgs->home.HomeFlags |= HOME_STOP_FIRST_LIM;

	if(m_ui->StopCondBox_2->currentIndex() == 0)
		lmStgs->home.HomeFlags |= HOME_STOP_SECOND_REV;
	if(m_ui->StopCondBox_2->currentIndex() == 1)
		lmStgs->home.HomeFlags |= HOME_STOP_SECOND_SYN;
	if(m_ui->StopCondBox_2->currentIndex() == 2)
		lmStgs->home.HomeFlags |= HOME_STOP_SECOND_LIM;

	if(m_ui->fastHomeBox->isChecked())
		lmStgs->home.HomeFlags |= HOME_USE_FAST;
}

void PageHomePositionWgt::FromClassToUi()
{	
	if (mStgs->entype.EngineType & ENGINE_TYPE_STEP)//(mStgs->engine.EngineFlags & MOTOR_TYPE_STEP)
	{
		// steps/usteps for stepper only
		m_ui->speedFastBox->setBaseValue(mStgs->home.FastHome, mStgs->home.uFastHome);
		m_ui->speedSlowBox->setBaseValue(mStgs->home.SlowHome, mStgs->home.uSlowHome);
		m_ui->deltaBox->setBaseValue(mStgs->home.HomeDelta, mStgs->home.uHomeDelta);
		
	}
	else
	{
		m_ui->speedFastBox->setBaseValue(mStgs->home.FastHome);
		m_ui->speedSlowBox->setBaseValue(mStgs->home.SlowHome);
		m_ui->deltaBox->setBaseValue(mStgs->home.HomeDelta);
		
	}

	if(mStgs->home.HomeFlags & HOME_DIR_FIRST)
		m_ui->dirFastBox->setCurrentIndex(1);
	else
		m_ui->dirFastBox->setCurrentIndex(0);

	if(mStgs->home.HomeFlags & HOME_DIR_SECOND)
		m_ui->dirBox->setCurrentIndex(1);
	else
		m_ui->dirBox->setCurrentIndex(0);

	m_ui->fromEndChk->setChecked(mStgs->home.HomeFlags & HOME_MV_SEC_EN);
	m_ui->halfRevChk->setChecked(mStgs->home.HomeFlags & HOME_HALF_MV);
	m_ui->halfRevChk->setEnabled(m_ui->fromEndChk->isChecked());

	if((mStgs->home.HomeFlags & HOME_STOP_FIRST_LIM) == HOME_STOP_FIRST_LIM)
		m_ui->StopCondBox->setCurrentIndex(2);
	else if(mStgs->home.HomeFlags & HOME_STOP_FIRST_SYN)
		m_ui->StopCondBox->setCurrentIndex(1);
	else if(mStgs->home.HomeFlags & HOME_STOP_FIRST_REV)
		m_ui->StopCondBox->setCurrentIndex(0);

	if((mStgs->home.HomeFlags & HOME_STOP_SECOND_LIM) == HOME_STOP_SECOND_LIM)
		m_ui->StopCondBox_2->setCurrentIndex(2);
	else if(mStgs->home.HomeFlags & HOME_STOP_SECOND_SYN)
		m_ui->StopCondBox_2->setCurrentIndex(1);
	else if(mStgs->home.HomeFlags & HOME_STOP_SECOND_REV)
		m_ui->StopCondBox_2->setCurrentIndex(0);

	m_ui->fastHomeBox->setChecked(mStgs->home.HomeFlags & HOME_USE_FAST);
}
