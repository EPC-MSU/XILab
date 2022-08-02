#include <pagestageencoderwgt.h>
#include <ui_pagestageencoderwgt.h>
#include <float.h>
#include <functions.h>

PageStageEncoderWgt::PageStageEncoderWgt(QWidget *parent, StageSettings *_stageStgs) :
    QWidget(parent),
    m_ui(new Ui::PageStageEncoderWgtClass)
{
    m_ui->setupUi(this);
	stageStgs = _stageStgs;

#ifndef SERVICEMODE
	QObject::connect(m_ui->encSetDifferentialOutput, SIGNAL(currentIndexChanged(int)), this, SLOT(OnDiffOutChanged(int)));
	QObject::connect(m_ui->encSetPushpullOutput, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPushOutChanged(int)));

	m_ui->manufacturerEdit->setReadOnly(true);
	m_ui->partnumberEdit->setReadOnly(true);

	m_ui->maxOperatingFrequencySpin->setReadOnly(true);
	m_ui->supplyVoltageMinSpin->setReadOnly(true);
	m_ui->supplyVoltageMaxSpin->setReadOnly(true);
	m_ui->maxCurrentConsumptionSpin->setReadOnly(true);
	m_ui->PPRSpin->setReadOnly(true);

//	m_ui->encSetDifferentialOutput->setReadOnly(true);
//	m_ui->encSetPushpullOutput->setReadOnly(true);
	m_ui->encSetIndexchannelPresent->setCheckable(false);
	m_ui->encSetRevolutionsensorPresent->setCheckable(false);
	m_ui->encSetRevolutionsensorActiveHigh->setCheckable(false);
#endif
}

PageStageEncoderWgt::~PageStageEncoderWgt()
{
    delete m_ui;
}

void PageStageEncoderWgt::SetDisabled(bool set)
{
	m_ui->manufacturerEdit->setDisabled(set);
	m_ui->partnumberEdit->setDisabled(set);

	m_ui->maxOperatingFrequencySpin->setDisabled(set);
	m_ui->supplyVoltageMinSpin->setDisabled(set);
	m_ui->supplyVoltageMaxSpin->setDisabled(set);
	m_ui->maxCurrentConsumptionSpin->setDisabled(set);
	m_ui->PPRSpin->setDisabled(set);

	m_ui->encSetDifferentialOutput->setDisabled(set);
	m_ui->encSetPushpullOutput->setDisabled(set);
	m_ui->encSetIndexchannelPresent->setDisabled(set);
	m_ui->encSetRevolutionsensorPresent->setDisabled(set);
	m_ui->encSetRevolutionsensorActiveHigh->setDisabled(set);

	if (set == true) {
		m_ui->manufacturerEdit->setText("");
		m_ui->partnumberEdit->setText("");

		m_ui->maxOperatingFrequencySpin->setValue(0);
		m_ui->supplyVoltageMinSpin->setValue(0);
		m_ui->supplyVoltageMaxSpin->setValue(0);
		m_ui->maxCurrentConsumptionSpin->setValue(0);
		m_ui->PPRSpin->setValue(0);

		m_ui->encSetDifferentialOutput->setCurrentIndex(-1);
		m_ui->encSetPushpullOutput->setCurrentIndex(-1);
		m_ui->encSetIndexchannelPresent->setChecked(false);
		m_ui->encSetRevolutionsensorPresent->setChecked(false);
		m_ui->encSetRevolutionsensorActiveHigh->setChecked(false);
	}
}

void PageStageEncoderWgt::FromClassToUi()
{
	m_ui->manufacturerEdit->setText(QString::fromUtf8(stageStgs->encoder_information.Manufacturer));
	m_ui->partnumberEdit->setText(QString::fromUtf8(stageStgs->encoder_information.PartNumber));

	m_ui->maxOperatingFrequencySpin->setValue(stageStgs->encoder_settings.MaxOperatingFrequency);
	m_ui->supplyVoltageMinSpin->setValue(stageStgs->encoder_settings.SupplyVoltageMin);
	m_ui->supplyVoltageMaxSpin->setValue(stageStgs->encoder_settings.SupplyVoltageMax);
	m_ui->maxCurrentConsumptionSpin->setValue(stageStgs->encoder_settings.MaxCurrentConsumption);
	m_ui->PPRSpin->setValue(stageStgs->encoder_settings.PPR);

	int selected;
	selected = ((stageStgs->encoder_settings.EncoderSettings & ENCSET_DIFFERENTIAL_OUTPUT) == 0);
	m_ui->encSetDifferentialOutput->setCurrentIndex(selected);
#ifndef SERVICEMODE
	ComboboxGrayout(m_ui->encSetDifferentialOutput, selected);
#endif

	selected = ((stageStgs->encoder_settings.EncoderSettings & ENCSET_PUSHPULL_OUTPUT) == 0);
	m_ui->encSetPushpullOutput->setCurrentIndex(selected);
#ifndef SERVICEMODE
	ComboboxGrayout(m_ui->encSetPushpullOutput, selected);
#endif

	m_ui->encSetIndexchannelPresent->setChecked(stageStgs->encoder_settings.EncoderSettings & ENCSET_INDEXCHANNEL_PRESENT);
	m_ui->encSetRevolutionsensorPresent->setChecked(stageStgs->encoder_settings.EncoderSettings & ENCSET_REVOLUTIONSENSOR_PRESENT);
	m_ui->encSetRevolutionsensorActiveHigh->setChecked(stageStgs->encoder_settings.EncoderSettings & ENCSET_REVOLUTIONSENSOR_ACTIVE_HIGH);
}

void PageStageEncoderWgt::FromUiToClass(StageSettings *lsStgs)
{
	safe_copy(lsStgs->encoder_information.Manufacturer, m_ui->manufacturerEdit->text().toUtf8().data());
	safe_copy(lsStgs->encoder_information.PartNumber, m_ui->partnumberEdit->text().toUtf8().data());

	lsStgs->encoder_settings.MaxOperatingFrequency = m_ui->maxOperatingFrequencySpin->value();
	lsStgs->encoder_settings.SupplyVoltageMin = m_ui->supplyVoltageMinSpin->value();
	lsStgs->encoder_settings.SupplyVoltageMax = m_ui->supplyVoltageMaxSpin->value();
	lsStgs->encoder_settings.MaxCurrentConsumption = m_ui->maxCurrentConsumptionSpin->value();
	lsStgs->encoder_settings.PPR = m_ui->PPRSpin->value();

	lsStgs->encoder_settings.EncoderSettings = 0;
	setUnsetBit((m_ui->encSetDifferentialOutput->currentIndex() == 0), &lsStgs->encoder_settings.EncoderSettings, ENCSET_DIFFERENTIAL_OUTPUT);
	setUnsetBit((m_ui->encSetPushpullOutput->currentIndex() == 0), &lsStgs->encoder_settings.EncoderSettings, ENCSET_PUSHPULL_OUTPUT);
	setUnsetBit(m_ui->encSetIndexchannelPresent->isChecked(), &lsStgs->encoder_settings.EncoderSettings, ENCSET_INDEXCHANNEL_PRESENT);
	setUnsetBit(m_ui->encSetRevolutionsensorPresent->isChecked(), &lsStgs->encoder_settings.EncoderSettings, ENCSET_REVOLUTIONSENSOR_PRESENT);
	setUnsetBit(m_ui->encSetRevolutionsensorActiveHigh->isChecked(), &lsStgs->encoder_settings.EncoderSettings, ENCSET_REVOLUTIONSENSOR_ACTIVE_HIGH);
}

void PageStageEncoderWgt::OnDiffOutChanged(int index)
{
	Q_UNUSED(index)
	m_ui->encSetDifferentialOutput->setCurrentIndex(stageStgs->encoder_settings.EncoderSettings & ENCSET_DIFFERENTIAL_OUTPUT);
}

void PageStageEncoderWgt::OnPushOutChanged(int index)
{
	Q_UNUSED(index)
	m_ui->encSetPushpullOutput->setCurrentIndex(stageStgs->encoder_settings.EncoderSettings & ENCSET_PUSHPULL_OUTPUT);
}