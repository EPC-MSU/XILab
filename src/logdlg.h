#ifndef LOGDLG_H
#define LOGDLG_H

#include <QDialog>
#include <QSettings>
#include <ui_logdlg.h>

class LogDlg : public QDialog
{
    Q_OBJECT

public:
    LogDlg(QWidget *parent, QString title);
    ~LogDlg();

	void FromUiToSettings(QSettings *settings);
	void FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);

private:
    Ui::LogDlgClass ui;

	QString default_load_path;
	QString default_save_path;

public slots:
	void close();
	void OnSaveBtnClicked();
};

#endif // LOGDLG_H
