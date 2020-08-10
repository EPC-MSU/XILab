#ifndef COMPARESETTINGSDIALOG_H
#define COMPARESETTINGSDIALOG_H

#include <QtGui/QWidget>
#include <QDialog>
#include <QSettings>

namespace Ui {
	class CompareSettingsDialogClass;
}

class CompareSettingsDialog : public QDialog {
public:
	CompareSettingsDialog(QWidget *parent, QStringList filenames);
	~CompareSettingsDialog();

	Ui::CompareSettingsDialogClass *m_ui;
};

#endif // COMPARESETTINGSDIALOG_H
