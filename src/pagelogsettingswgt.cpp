#include <pagelogsettingswgt.h>
#include <ui_pagelogsettingswgt.h>

PageLogSettingsWgt::PageLogSettingsWgt(QWidget *parent, LogSettings *logStgs) :
	QWidget(parent),
	m_ui(new Ui::PageLogSettingsWgtClass)
{
	lStgs = logStgs;
    m_ui->setupUi(this);
	m_ui->AutosavePath->setEnabled(m_ui->AutosaveChk->isChecked());
}

PageLogSettingsWgt::~PageLogSettingsWgt()
{
	delete m_ui;
}

void PageLogSettingsWgt::FromUiToSettings(QSettings *settings)
{
	settings->beginGroup("Log_setup");

	settings->setValue("Xilab_loglevel", m_ui->comboBoxXLL->currentIndex());
	settings->setValue("script_loglevel", m_ui->comboBoxSLL->currentIndex());
	settings->setValue("library_loglevel", m_ui->comboBoxLLL->currentIndex());
	settings->setValue("Autosave_enabled", m_ui->AutosaveChk->isChecked());
	settings->setValue("Autosave_path", m_ui->AutosavePath->text());

	settings->endGroup();
}

QString PageLogSettingsWgt::FromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	QString errors;

	settings->beginGroup("Log_setup");
	if(default_stgs != NULL)
		default_stgs->beginGroup("Log_setup");

	if(settings->contains("Xilab_loglevel"))
		m_ui->comboBoxXLL->setCurrentIndex(settings->value("Xilab_loglevel", 3).toInt());
	else if(default_stgs != NULL)
		m_ui->comboBoxXLL->setCurrentIndex(default_stgs->value("Xilab_loglevel", 3).toInt());
	else 
		errors+="\"Xilab_loglevel\" key is not found\n";

	if(settings->contains("script_loglevel"))
		m_ui->comboBoxSLL->setCurrentIndex(settings->value("script_loglevel", 3).toInt());
	else if(default_stgs != NULL)
		m_ui->comboBoxSLL->setCurrentIndex(default_stgs->value("script_loglevel", 3).toInt());
	else 
		errors+="\"script_loglevel\" key is not found\n";

	if(settings->contains("library_loglevel"))
		m_ui->comboBoxLLL->setCurrentIndex(settings->value("library_loglevel", 3).toInt());
	else if(default_stgs != NULL)
		m_ui->comboBoxLLL->setCurrentIndex(default_stgs->value("library_loglevel", 3).toInt());
	else 
		errors+="\"library_loglevel\" key is not found\n";

	if(settings->contains("Autosave_enabled"))
		m_ui->AutosaveChk->setChecked(settings->value("Autosave_enabled", true).toBool());
	else if(default_stgs != NULL)
		m_ui->AutosaveChk->setChecked(default_stgs->value("Autosave_enabled", true).toBool());
	else 
		errors+="\"Autosave_enabled\" key is not found\n";

	if(settings->contains("Autosave_path"))
		m_ui->AutosavePath->setText(settings->value("Autosave_path", ".").toString());
	else if(default_stgs != NULL)
		m_ui->AutosavePath->setText(default_stgs->value("Autosave_path", ".").toString());
	else 
		errors+="\"Autosave_path\" key is not found\n";

	settings->endGroup();
	if(default_stgs != NULL)
		default_stgs->endGroup();

	return errors;
}

void PageLogSettingsWgt::FromClassToUi()
{
	m_ui->comboBoxXLL->setCurrentIndex(lStgs->xll_index);
	m_ui->comboBoxSLL->setCurrentIndex(lStgs->sll_index);
	m_ui->comboBoxLLL->setCurrentIndex(lStgs->lll_index);
	m_ui->AutosaveChk->setChecked(lStgs->autosave_enabled);
	m_ui->AutosavePath->setText(lStgs->autosave_path.path());
}

void PageLogSettingsWgt::FromUiToClass()
{
	lStgs->xll_index = m_ui->comboBoxXLL->currentIndex();
	lStgs->sll_index = m_ui->comboBoxSLL->currentIndex();
	lStgs->lll_index = m_ui->comboBoxLLL->currentIndex();
	lStgs->autosave_enabled = m_ui->AutosaveChk->isChecked();
	lStgs->autosave_path = QDir(m_ui->AutosavePath->text());
}