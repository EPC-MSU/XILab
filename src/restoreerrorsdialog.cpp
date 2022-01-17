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

	QFontMetrics font_metrics(m_ui->textBrowser->font());
	int font_height = font_metrics.height();

	// Get the height by multiplying number of lines by font height, Maybe add to this a bit for a slight margin?
	int height = font_height * 7.5;

	// Set the height to the text broswer
	m_ui->textBrowser->setMinimumHeight(height);
	m_ui->textBrowser->setMaximumHeight(height);
	

	QFontMetrics font_metricslist(m_ui->listWidget->font());
	int font_heightlist = font_metricslist.height();

	// Get the height by multiplying number of lines by font height, Maybe add to this a bit for a slight margin?
	int heightlist = font_heightlist * 5;

	int widgheigt = height + /*m_ui->label->size().height()*/110 + heightlist + 80;
	this->resize(550, widgheigt);

	this->setMinimumSize(QSize(this->width(), widgheigt));
	this->setMaximumSize(QSize(this->width()+50, widgheigt));
	this->setMaximumSize(QSize(this->width()+50, 16777215));

	//m_ui->textBrowser->setFixedHeight(m_ui->textBrowser->document()->size().height());
	
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