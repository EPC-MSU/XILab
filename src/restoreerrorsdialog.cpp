#include <ui_restoreerrorsdialog.h>
#include <restoreerrorsdialog.h>
#include <QTimer>

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


	//timer = new QTimer();
	//connect(timer, SIGNAL(timeout()), this, SLOT(restartmovie()));
	//timer->start(2000);

	//movie.start();

	while (movie.state() == QMovie::NotRunning)
	{
		movie.start();
	}
	
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