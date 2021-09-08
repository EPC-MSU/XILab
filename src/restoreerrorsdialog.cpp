#include <ui_restoreerrorsdialog.h>
#include <restoreerrorsdialog.h>
#include <QTimer>
#include "windows.h"


RestoreErrorsDialog::RestoreErrorsDialog(QWidget *parent, QStringList items) :
	QDialog(parent),
	m_ui(new Ui::RestoreErrorsDialogClass)
{
	m_ui->setupUi(this);
	m_ui->listWidget->addItems(items);


	movie.setFileName(":/settingsdlg/images/settingsdlg/warning.gif");
	movie.setSpeed(25);

	m_ui->label->setMovie(&movie); // label имеет тип QLabel*

	//QObject::connect(this, SIGNAL(movie.finished()), this, SLOT(movie.start()));
	
	movie.start();

	//Sleep(5000);
	//timer = new QTimer();
	//connect(timer, SIGNAL(timeout()), this, SLOT(restartmovie()));
	//movie.start();

	timer.start(4000);
	connect(&timer, SIGNAL(timeout()), this, SLOT(restartmovie()), Qt::DirectConnection);
	//movie.start();

	/*while (movie.state() == QMovie::NotRunning)
	{
		movie.start();
	}
	*/
}

RestoreErrorsDialog::~RestoreErrorsDialog()
{
	delete m_ui;
}

void RestoreErrorsDialog::restartmovie()
{
	movie.stop();
	//movie.setFileName(":/settingsdlg/images/settingsdlg/warning.gif");
	//movie.setSpeed(25);

	//m_ui->label->setMovie(&movie); // label имеет тип QLabel*
	movie.start();
}