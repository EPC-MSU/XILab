#include <pagedebugwgt.h>
#include <ui_pagedebugwgt.h>
#include <deviceinterface.h>
#include <float.h>

const int bytes = 128;
const int boxes = 4;

nonvolatile_memory_t nvm;
const int nvm_boxes = 1 + (sizeof(nvm.UserData) / sizeof(nvm.UserData[0]));
const int nvm_rows = 2;
const int nvm_columns = nvm_boxes / nvm_rows;

PageDebugWgt::PageDebugWgt(QWidget *parent, MotorSettings *motorStgs, char* dev_name, DeviceInterface *_devinterface) :
    QWidget(parent),
    m_ui(new Ui::PageDebugWgtClass)
{
	m_ui->setupUi(this);
	mStgs = motorStgs;
	strcpy(device_name, dev_name);
	devinterface = _devinterface;

	QList<QPair<qint64, qint64> > bound;
	bound
		<< QPair<qint64, qint64>(LLONG_MIN, LLONG_MAX)
		<< QPair<qint64, qint64>(LLONG_MIN, LLONG_MAX)
		<< QPair<qint64, qint64>(0, ULONG_MAX)
		<< QPair<qint64, qint64>(0,	ULONG_MAX)
		<< QPair<qint64, qint64>(LONG_MIN, LONG_MAX)
		<< QPair<qint64, qint64>(LONG_MIN, LONG_MAX)
			;
	for (int i=0; i<boxes; i++) {
		for (int k=0; k<6; k++) {
			QLabel *label = new QLabel();
			label->setObjectName("label_" + toStr(i) + "_" + toStr(k));
			m_ui->gridLayout_getData->addWidget(label, i+1, k);
			LongLongSpinBox *box = new LongLongSpinBox();
			box->setRange(bound.at(k).first, bound.at(k).second);
			m_ui->gridLayout_setData->addWidget(box, i+1, k);
			box->setObjectName("box_" + toStr(i) + "_" + toStr(k));
		}
	}

	// xNVM box setup
	for (int i=0; i<nvm_boxes; i++) {
		QSpinBox *box = new QSpinBox();
		box->setRange(0, UINT_MAX);
		m_ui->gridLayout_NVM->addWidget(box, 1 + i / nvm_columns, i % nvm_columns);
		box->setObjectName("nvm_box_" + toStr(i));
	}

	QObject::connect(m_ui->dbgrBtn, SIGNAL(clicked()), this, SLOT(OnDbgr()));
	QObject::connect(m_ui->dbgwBtn, SIGNAL(clicked()), this, SLOT(OnDbgw()));
	QObject::connect(m_ui->copyBtn, SIGNAL(clicked()), this, SLOT(OnCopy()));
	QObject::connect(m_ui->sposBtn, SIGNAL(clicked()), this, SLOT(OnSpos()));
	QObject::connect(m_ui->gposBtn, SIGNAL(clicked()), this, SLOT(OnGpos()));
	QObject::connect(m_ui->snvmBtn, SIGNAL(clicked()), this, SLOT(OnSnvm()));
	QObject::connect(m_ui->gnvmBtn, SIGNAL(clicked()), this, SLOT(OnGnvm()));
	QObject::connect(m_ui->loftBtn, SIGNAL(clicked()), this, SLOT(OnLoft()));
	QObject::connect(m_ui->pwofBtn, SIGNAL(clicked()), this, SLOT(OnPwof()));
	QObject::connect(m_ui->eesvBtn, SIGNAL(clicked()), this, SLOT(OnEesv()));
	QObject::connect(m_ui->eerdBtn, SIGNAL(clicked()), this, SLOT(OnEerd()));
	QObject::connect(m_ui->restBtn, SIGNAL(clicked()), this, SLOT(OnRest()));
	QObject::connect(m_ui->clfrBtn, SIGNAL(clicked()), this, SLOT(OnClfr()));
	QObject::connect(m_ui->fixBtn, SIGNAL(clicked()), this, SLOT(OnFix()));
	QObject::connect(m_ui->callpidBtn, SIGNAL(clicked()), this, SLOT(OnPidCalibration()));

	m_ui->posSpinBox->setUnitType(UserUnitSettings::TYPE_COORD);
}

PageDebugWgt::~PageDebugWgt()
{
    delete m_ui;
}

void PageDebugWgt::OnPidCalibration()
{

}

void PageDebugWgt::OnDbgr()
{
	if (devinterface->get_debug_read(&debug_read) == result_ok) {
		for (int i=0; i<bytes; i++)
			chars[i] = debug_read.DebugData[i];
		redraw();
	}
}

void PageDebugWgt::OnDbgw()
{
	for (int i=0; i<boxes; i++) {
#if defined(__LINUX__)
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
		
		*(signed long long*) &chars[i*bytes/boxes + 0] = findChild<LongLongSpinBox*>("box_" + toStr(i) + "_0")->value();
		*(signed long long*) &chars[i*bytes/boxes + 8] = findChild<LongLongSpinBox*>("box_" + toStr(i) + "_1")->value();
		*(unsigned int*) &chars[i*bytes/boxes + 16] = findChild<LongLongSpinBox*>("box_" + toStr(i) + "_2")->value();
		*(unsigned int*) &chars[i*bytes/boxes + 20] = findChild<LongLongSpinBox*>("box_" + toStr(i) + "_3")->value();
		*(unsigned int*) &chars[i*bytes/boxes + 24] = findChild<LongLongSpinBox*>("box_" + toStr(i) + "_4")->value();
		*(unsigned int*) &chars[i*bytes/boxes + 28] = findChild<LongLongSpinBox*>("box_" + toStr(i) + "_5")->value();
#if defined(__LINUX__)
#pragma GCC diagnostic warning "-Wstrict-aliasing"
#endif

	}

	for (int i=0; i<bytes; i++)
		debug_write.DebugData[i] = chars[i];
	devinterface->set_debug_write(&debug_write);
}

void PageDebugWgt::OnCopy()
{
	for (int i=0; i<boxes; i++) {
		for (int j=0; j<6; j++) { // another hardcode ;(
			findChild<LongLongSpinBox*>("box_" + toStr(i) + "_" + toStr(j))->setValue(findChild<QLabel*>("label_" + toStr(i) + "_" + toStr(j))->text().toLongLong());
		}
	}
}

void PageDebugWgt::OnSpos() {
	set_position_t spos;
	spos.Position = m_ui->posSpinBox->getBaseStep();
	spos.uPosition = m_ui->posSpinBox->getBaseUStep();
	spos.EncPosition = m_ui->encSpinBox->value();
	spos.PosFlags = SETPOS_IGNORE_POSITION * (m_ui->ignorePosCheckBox->isChecked()) +
					SETPOS_IGNORE_ENCODER * (m_ui->ignoreEncCheckBox->isChecked());
	devinterface->set_position(&spos);
}

void PageDebugWgt::OnGpos() {
	get_position_t gpos;
	if (devinterface->get_position(&gpos) == result_ok) {
		m_ui->label_pos->setText(toStr(gpos.Position) + QString(" ") + toStr(gpos.uPosition) + QString("/") + toStr(mStgs->getStepFrac()));
		m_ui->label_enc->setText(toStr(gpos.EncPosition));
	}
}

void PageDebugWgt::OnLoft() {
	devinterface->command_loft();
}

void PageDebugWgt::OnPwof() {
	devinterface->command_power_off();
}
void PageDebugWgt::OnEesv() {
	emit EESV();
	devinterface->command_eesave_settings();
}

void PageDebugWgt::OnEerd() {
	devinterface->command_eeread_settings();
	emit EERD();
}

void PageDebugWgt::OnRest() {
	devinterface->command_reset();
}

void PageDebugWgt::OnClfr() {
	devinterface->command_clear_fram();
}

void PageDebugWgt::OnSnvm() {
	nonvolatile_memory_t nvm;
	for (int i=0; i<nvm_boxes; i++) {
		nvm.UserData[i] = findChild<QSpinBox*>("nvm_box_" + toStr(i))->value();
	}
	devinterface->set_nonvolatile_memory(&nvm);
}

void PageDebugWgt::OnGnvm() {
	nonvolatile_memory_t nvm;
	if (devinterface->get_nonvolatile_memory(&nvm) == result_ok) {
		for (int i=0; i<nvm_boxes; i++) {
			findChild<QSpinBox*>("nvm_box_" + toStr(i))->setValue(nvm.UserData[i]);
		}
	}
}

void PageDebugWgt::OnFix() {
#if defined(WIN32) || defined(WIN64)
	if (devinterface->is_open())
		devinterface->close_device();
	ximc_fix_usbser_sys(device_name);
#endif
}

void PageDebugWgt::redraw()
{
	for (int i=0; i<boxes; i++) {
#if defined(__LINUX__)
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
		findChild<QLabel*>("label_"+toStr(i)+"_0")->setText(toStr(*(signed long long*)&chars[i*bytes/boxes + 0]));
		findChild<QLabel*>("label_"+toStr(i)+"_1")->setText(toStr(*(signed long long*)&chars[i*bytes/boxes + 8]));
		findChild<QLabel*>("label_"+toStr(i)+"_2")->setText(toStr(*(unsigned int*)&chars[i*bytes/boxes + 16]));
		findChild<QLabel*>("label_"+toStr(i)+"_3")->setText(toStr(*(unsigned int*)&chars[i*bytes/boxes + 20]));
		findChild<QLabel*>("label_"+toStr(i)+"_4")->setText(toStr(*(signed int*)&chars[i*bytes/boxes + 24]));
		findChild<QLabel*>("label_"+toStr(i)+"_5")->setText(toStr(*(signed int*)&chars[i*bytes/boxes + 28]));
#if defined(__LINUX__)
#pragma GCC diagnostic warning "-Wstrict-aliasing"
#endif
	}
}
