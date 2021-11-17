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

#if defined(__LINUX__)
	m_ui->textBrowser->setMinimumHeight(100);
	m_ui->textBrowser ->setMaximumHeight(100);
	this->resize(550, 265);
	this->setMinimumSize(QSize(550, 265));
	this->setMaximumSize(QSize(600, 265));
	this->setMaximumSize(QSize(600, 16777215));
#endif

#if defined(__APPLE__)
	m_ui->textBrowser->setMinimumHeight(100);
	m_ui->textBrowser->setMaximumHeight(100);
	this->resize(550, 330);
	this->setMinimumSize(QSize(550, 330));
	this->setMaximumSize(QSize(600, 330));
	this->setMaximumSize(QSize(600, 16777215));
#endif


	//movie->setSpeed(25);
	//m_ui->label->setMovie(movie); // label имеет тип QLabel*
	//movie->setCacheMode(QMovie::CacheAll);
	//movie->start();

	movie.setFileName(":/settingsdlg/images/settingsdlg/warning.gif");
	m_ui->label->setMovie(&movie);
	movie.setSpeed(25);
	movie.setCacheMode(QMovie::CacheAll);
	movie.start();
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