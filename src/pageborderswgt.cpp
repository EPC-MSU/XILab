#include <pageborderswgt.h>
#include <ui_pageborderswgt.h>

PageBordersWgt::PageBordersWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageBordersWgtClass)
{
    m_ui->setupUi(this);

    mStgs = motorStgs;

    QObject::connect(m_ui->border1Lst, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSw1CurChanged(int)));
    QObject::connect(m_ui->border2Lst, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSw2CurChanged(int)));

	m_ui->leftBdrValue->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->rightBdrValue->setUnitType(UserUnitSettings::TYPE_COORD);
}

PageBordersWgt::~PageBordersWgt()
{
    delete m_ui;
}

void PageBordersWgt::FromClassToUi()
{
    m_ui->switchBtn->setChecked(!(mStgs->edges.BorderFlags & BORDER_IS_ENCODER));
    m_ui->encoderBtn->setChecked(mStgs->edges.BorderFlags & BORDER_IS_ENCODER);

    m_ui->stopLeftChk->setChecked(mStgs->edges.BorderFlags & BORDER_STOP_LEFT);
    m_ui->stopRightChk->setChecked(mStgs->edges.BorderFlags & BORDER_STOP_RIGHT);
    m_ui->borderMissetChk->setChecked(mStgs->edges.BorderFlags & BORDERS_SWAP_MISSET_DETECTION);

    m_ui->pushed1Lst->setCurrentIndex(!(mStgs->edges.EnderFlags & ENDER_SW1_ACTIVE_LOW));
    m_ui->pushed2Lst->setCurrentIndex(!(mStgs->edges.EnderFlags & ENDER_SW2_ACTIVE_LOW));

    m_ui->border1Lst->setCurrentIndex(mStgs->edges.EnderFlags & ENDER_SWAP);
	m_ui->border2Lst->setCurrentIndex(!(mStgs->edges.EnderFlags & ENDER_SWAP));

	m_ui->leftBdrValue->setBaseValue(mStgs->edges.LeftBorder, mStgs->edges.uLeftBorder);
	m_ui->rightBdrValue->setBaseValue(mStgs->edges.RightBorder, mStgs->edges.uRightBorder);
}

void PageBordersWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->edges.BorderFlags = 0;
	lmStgs->edges.EnderFlags = 0; 

	setUnsetBit(m_ui->encoderBtn->isChecked(), &lmStgs->edges.BorderFlags, BORDER_IS_ENCODER);
	setUnsetBit(m_ui->stopLeftChk->isChecked(), &lmStgs->edges.BorderFlags, BORDER_STOP_LEFT);
	setUnsetBit(m_ui->stopRightChk->isChecked(), &lmStgs->edges.BorderFlags, BORDER_STOP_RIGHT);
	setUnsetBit(m_ui->borderMissetChk->isChecked(), &lmStgs->edges.BorderFlags, BORDERS_SWAP_MISSET_DETECTION);

	setUnsetBit((m_ui->pushed1Lst->currentIndex() == 0), &lmStgs->edges.EnderFlags, ENDER_SW1_ACTIVE_LOW);
	setUnsetBit((m_ui->pushed2Lst->currentIndex() == 0), &lmStgs->edges.EnderFlags, ENDER_SW2_ACTIVE_LOW);
	setUnsetBit((m_ui->border1Lst->currentIndex() == 1), &lmStgs->edges.EnderFlags, ENDER_SWAP);

	lmStgs->edges.LeftBorder = m_ui->leftBdrValue->getBaseStep();
	lmStgs->edges.uLeftBorder = m_ui->leftBdrValue->getBaseUStep();
	lmStgs->edges.RightBorder = m_ui->rightBdrValue->getBaseStep();
	lmStgs->edges.uRightBorder = m_ui->rightBdrValue->getBaseUStep();
}

void PageBordersWgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PageBordersWgt::OnSw1CurChanged(int index)
{
    if (m_ui->border2Lst->currentIndex() == index) m_ui->border2Lst->setCurrentIndex(!index);
}

void PageBordersWgt::OnSw2CurChanged(int index)
{
    if (m_ui->border1Lst->currentIndex() == index) m_ui->border1Lst->setCurrentIndex(!index);
}

QString PageBordersWgt::GetBordersType()
{
	if(m_ui->encoderBtn->isChecked()) return "encoder";
	else if(m_ui->switchBtn->isChecked()) return "switch";
	else return "unknown";
}
