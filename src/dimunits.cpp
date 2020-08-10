#include <dimunits.h>

DimensionalUnitsDlg::DimensionalUnitsDlg(QWidget *parent, QString title, QStringList serials, double delay)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(this,			SIGNAL(finished(int)), this, SLOT(close()));
	QObject::connect(ui.buttonBox,	SIGNAL(accepted()), this, SLOT(onOKPressed()));
	QObject::connect(ui.buttonBox,	SIGNAL(rejected()), this, SLOT(onCancelPressed()));
	QObject::connect(ui.pushButton_up,	SIGNAL(clicked()), this, SLOT(onBtnUpPressed()));
	QObject::connect(ui.pushButton_down,SIGNAL(clicked()), this, SLOT(onBtnDownPressed()));

	QString wndTitle = title;
	setWindowTitle(wndTitle);

	ui.listWidget->addItems(serials);
	ui.lineEraseDelaySpin->setValue(delay);
}

DimensionalUnitsDlg::~DimensionalUnitsDlg()
{
}

void DimensionalUnitsDlg::onOKPressed()
{
	QStringList str;
	for(int i = 0; i < ui.listWidget->count(); ++i) {
		str.append(ui.listWidget->item(i)->text());
	}
	double delay = ui.lineEraseDelaySpin->value();
	emit sendSerials(str, delay);
	this->accept();
}

void DimensionalUnitsDlg::onCancelPressed()
{
	this->reject();
}

void DimensionalUnitsDlg::onBtnUpPressed()
{
	if (ui.listWidget->selectedItems().count() == 1) {
		int currentIndex = ui.listWidget->currentRow();
		if (currentIndex > 0) {
			QListWidgetItem *currentItem = ui.listWidget->takeItem(currentIndex);
			ui.listWidget->insertItem(currentIndex-1, currentItem);
			ui.listWidget->setCurrentRow(currentIndex-1);
		}
	}
}

void DimensionalUnitsDlg::onBtnDownPressed()
{
	if (ui.listWidget->selectedItems().count() == 1) {
		int currentIndex = ui.listWidget->currentRow();
		if (currentIndex < ui.listWidget->count()-1) {
			QListWidgetItem *currentItem = ui.listWidget->takeItem(currentIndex);
			ui.listWidget->insertItem(currentIndex+1, currentItem);
			ui.listWidget->setCurrentRow(currentIndex+1);
		}
	}
}

void DimensionalUnitsDlg::insertWidget(QWidget* widget)
{
	((QGridLayout*)ui.groupBox_3->layout())->addWidget(widget);
}
