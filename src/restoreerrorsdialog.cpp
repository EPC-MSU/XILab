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


	movie.setFileName(":/settingsdlg/images/settingsdlg/warning.gif");
	m_ui->label->setMovie(&movie);
	movie.setSpeed(25);
	movie.setCacheMode(QMovie::CacheAll);
	int height_label = 4 * m_ui->label->height();
	float Kos(1.0f);
#if defined(__LINUX__)
	height_label = 20;
#endif

#if defined(__APPLE__)
	Kos = 1.25f;
#endif

	QFontMetrics font_metrics(m_ui->textBrowser->font());
	int font_height = font_metrics.height();

	// Get the height by multiplying number of lines by font height, Maybe add to this a bit for a slight margin?
	int height = font_height * 8;
	float scale_font = Kos * font_height / 15;

	// Set the height to the text broswer
	m_ui->textBrowser->setMinimumHeight(height);
	m_ui->textBrowser->setMaximumHeight(height);
	

	QFontMetrics font_metricslist(m_ui->listWidget->font());
	int font_heightlist = font_metricslist.height();

	// Get the height by multiplying number of lines by font height, Maybe add to this a bit for a slight margin?
	int heightlist = scale_font*font_heightlist * 5;

	//int height_label = 4*m_ui->label->height();

	int widgheigt = height + height_label + heightlist + (int)(scale_font * 50) + m_ui->buttonBox->height();
	this->resize((int)(scale_font*this->width()), widgheigt);

	this->setMinimumSize(QSize((int) (scale_font*this->width()), widgheigt));
	this->setMaximumSize(QSize((int) (scale_font*this->width() + 50), widgheigt));
	this->setMaximumSize(QSize((int) (scale_font*this->width() + 50), 16777215));

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
		m_ui->label->setMovie(movie); // label ����� ��� QLabel*
		connect(movie, SIGNAL(error(error_gif)), this, SLOT(restartmovie()));
		movie->start();
	}
}*/