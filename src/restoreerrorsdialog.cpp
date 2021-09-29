#include <ui_restoreerrorsdialog.h>
#include <restoreerrorsdialog.h>
#include <QTimer>


RestoreErrorsDialog::RestoreErrorsDialog(QWidget *parent, QStringList items) :
	QDialog(parent),
	m_ui(new Ui::RestoreErrorsDialogClass),
	movie(new QMovie(":/settingsdlg/images/settingsdlg/warning.gif"))
{
	m_ui->setupUi(this);
	m_ui->listWidget->addItems(items);
	//movie->setFileName(":/settingsdlg/images/settingsdlg/warning.gif");
	movie->setSpeed(25);
	//movie->setScaledSize(QSize(1.25, 1.25));
	m_ui->label->setMovie(movie); // label имеет тип QLabel*
	//connect(movie, SIGNAL(finished()), this, SLOT(restartmovie()));
	movie->start();
}

RestoreErrorsDialog::~RestoreErrorsDialog()
{
	delete m_ui;
}

/*void RestoreErrorsDialog::restartmovie()
{
	//if (movie) 
	movie->~QMovie();

	movie = new QMovie(":/settingsdlg/images/settingsdlg/warning.gif");
	if (movie->isValid())
	{
		m_ui->label->setMovie(movie); // label имеет тип QLabel*
		connect(movie, SIGNAL(error(error_gif)), this, SLOT(restartmovie()));
		movie->start();
	}
}*/