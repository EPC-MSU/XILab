#include <ui_restoreerrorsdialog.h>
#include <restoreerrorsdialog.h>

RestoreErrorsDialog::RestoreErrorsDialog(QWidget *parent, QStringList items) :
	QDialog(parent),
	m_ui(new Ui::RestoreErrorsDialogClass)
{
	m_ui->setupUi(this);
	m_ui->listWidget->addItems(items);
}

RestoreErrorsDialog::~RestoreErrorsDialog()
{
	delete m_ui;
}