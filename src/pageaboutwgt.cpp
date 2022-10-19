#include <pageaboutwgt.h>
#include <ui_pageaboutwgt.h>
#include <ximc.h>
#include <QMessageBox>
#include <QFile>
#include "functions.h"
#include <QRegExp>
#include <QDir>
#include <loggedfile.h>

extern bool save_configs;
PageAboutWgt::PageAboutWgt(QWidget *parent)
    : QWidget(parent),
	m_ui(new Ui::PageAboutWgtClass)
{
	m_ui->setupUi(this);

	m_ui->versionValue->setText(xilab_ver);//xilab_version
	m_ui->versionValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_ui->qt_verLbl->setText(QString("Qt version:"));//qt_version
	m_ui->label_qt->setText(QString(qVersion()));//qt_version
	m_ui->label_qt->setTextInteractionFlags(Qt::TextSelectableByMouse);
	ximc_ver = new char[32+1];
	ximc_version(ximc_ver);	
	m_ui->versionValue_lib->setText(ximc_ver);//lib_version
	m_ui->versionValue_lib->setTextInteractionFlags(Qt::TextSelectableByMouse);
	m_ui->update_label->setOpenExternalLinks(true);
	connect(m_ui->delAll, SIGNAL(clicked()), this, SLOT(OnRemoveAllConfigsBtnClicked()));
}

PageAboutWgt::~PageAboutWgt()
{
	delete ximc_ver;
	delete m_ui;
}

void PageAboutWgt::OnRemoveAllConfigsBtnClicked()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(m_ui->delAll->text());
	msgBox.setText("This will remove all saved configuration files for all controllers and reset all XiLab settings to their defaults.\nXiLab will then close and will need to be reopened.");
	msgBox.setInformativeText("Do you wish to continue?");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);

	QAbstractButton *okBtn = msgBox.button(QMessageBox::Ok);
	msgBox.exec();

	if (msgBox.clickedButton() == okBtn) {
		// removes settings.ini
		LoggedFile::remove(MakeProgramConfigFilename());
		
		// removes *.cfg and V_*
		QDir dir = getDefaultPath();
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		QFileInfoList list = dir.entryInfoList();
		QRegExp rx_cfg("..[0-9]{1,10}.cfg"); // not exactly an uint match, but close enough
		QRegExp rx_vmem("V_[0-9]{1,10}"); // same here
		for (int i = 0; i < list.size(); ++i) {
			QString fn = list.at(i).fileName();
			if (rx_cfg.exactMatch(fn) || rx_vmem.exactMatch(fn)) { // checking whether we should delete it
				LoggedFile file(list.at(i).absoluteFilePath());
				file.remove();
			}
		}

		// removes scratch.txt
		LoggedFile::remove(DefaultScriptScratchName());

		// also delete keyfile.sqlite
		LoggedFile::remove(BindyKeyfileName());
		
		save_configs = false;
		emit exit_on_remove();
	}

}
