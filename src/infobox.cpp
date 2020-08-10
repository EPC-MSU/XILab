#include <infobox.h>
#include <QDesktopWidget>
#include <ui_infobox.h>

InfoBox::InfoBox(QWidget *parent): QDialog(parent), ui(new Ui::InfoBoxClass)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

	pressed = false;
	closingOnReject = true;

	ui->textLabel->clear();
	ui->movieLabel->clear();
	ui->iconLabel->clear();

	movie.setFileName(":/devicedlg/images/devicedlg/wait.gif");
	ui->movieLabel->setMovie(&movie);
	movie.setSpeed(250);
	movie.start();

	ui->buttonBox->setStandardButtons(QDialogButtonBox::NoButton);

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonPressed(QAbstractButton*)));

	QPoint pos(QApplication::desktop()->screen(0)->width()/2, QApplication::desktop()->screen(0)->height()/2);
	move(pos.x() - this->width()/2, pos.y() - this->height());
}

InfoBox::~InfoBox(){}

void InfoBox::show()
{
	QWidget::show();
	QApplication::processEvents();
}

void InfoBox::buttonPressed(QAbstractButton *button)
{
	switch (ui->buttonBox->standardButton(button)){
		case QDialogButtonBox::Ok:		emit accepted();
										pressed = true;
										close();
										break;

		case QDialogButtonBox::Cancel:	emit rejected();
										emit cancelled();
										pressed = true;
										if(closingOnReject)
											close();
										else
											hide();
										break;

		default:						pressed = true;
										close();
	}
}

void InfoBox::setIcon(QMessageBox::Icon icon)
{
	pressed = false;

	switch(icon){
		case QMessageBox::NoIcon:		ui->iconLabel->setVisible(false);
										break;
		
		case QMessageBox::Critical:		ui->iconLabel->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical).pixmap(32));
										ui->iconLabel->setVisible(true);
										break;
		
		case QMessageBox::Information:	ui->iconLabel->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(32));
										ui->iconLabel->setVisible(true);
										break;
		
		case QMessageBox::Question:		ui->iconLabel->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(32));
										ui->iconLabel->setVisible(true);
										break;
		
		case QMessageBox::Warning:		ui->iconLabel->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(32));
										ui->iconLabel->setVisible(true);
										break;
	}
}

void InfoBox::setText(QString text)
{
	ui->textLabel->setText(text);
	adjustSize();
	pressed = false;
}

void InfoBox::setButtons(QDialogButtonBox::StandardButtons buttons)
{
	ui->buttonBox->setStandardButtons(buttons);
	setButtonsVisible(true);
	adjustSize();
	pressed = false;
}

void InfoBox::setButtonsVisible(bool on){ ui->buttonBox->setVisible(on); adjustSize(); }
void InfoBox::setIconVisible(bool on){ ui->iconLabel->setVisible(on); adjustSize(); }
void InfoBox::setMovieVisible(bool on){ ui->movieLabel->setVisible(on); adjustSize(); }
void InfoBox::setTextVisible(bool on){ ui->textLabel->setVisible(on); adjustSize(); }
