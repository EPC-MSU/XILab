#include <logdlg.h>
#include <mainwindow.h>
#include <functions.h>
#include <QFileDialog>
#include <QMessageBox>
#include <loggedfile.h>

LogDlg::LogDlg(QWidget *parent, QString title)
    : QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.saveBtn, SIGNAL(clicked()), this, SLOT(OnSaveBtnClicked()));
	connect(ui.clearBtn, SIGNAL(clicked()), parent, SLOT(OnClearLogBtnPressed()));
	QString wndTitle = "Multiaxis log " + title;
	setWindowTitle(wndTitle);
 }

LogDlg::~LogDlg()
{
}

void LogDlg::close()
{
}

void LogDlg::OnSaveBtnClicked()
{	// Copypasted from MainWindow save log function
	if(default_save_path == "")
		default_save_path = getDefaultScriptsPath();

	QString filename;
	QDateTime now = QDateTime::currentDateTime();
	filename = QFileDialog::getSaveFileName(NULL, tr("Save log file"), getDefaultPath()+"/log_"+now.toString("yyyy.MM.dd_hh.mm.ss")+".csv", tr("CSV files (*.csv);;Text files (*.txt);;All files (*.*)"));

	if(filename.isEmpty()) {
		return;
	}

	LoggedFile file(filename);
	file.open(QIODevice::WriteOnly);

	QStringList strList;
	for (int i = 0; i < ui.logEdit->rowCount(); ++i) {
		for (int j =0; j < ui.logEdit->columnCount(); ++j) {
			strList << "\"" + ui.logEdit->item(i, j)->text() + "\",";
		}
		strList.back().chop(1); // Remove last comma
		strList << "\r\n";
	}
	file.write( strList.join("").toUtf8().data() );
	file.close();
}

void LogDlg::FromUiToSettings(QSettings *settings)
{
	//положение окна
	settings->beginGroup("logWindow_params");
	settings->setValue("position", pos());
	settings->setValue("size", size());
	settings->setValue("last_visible", isVisible());
    //дефолтные пути
    settings->setValue("load_path", default_load_path);
    settings->setValue("save_path", default_save_path);

	settings->endGroup();
}
void LogDlg::FromSettingsToUi(QSettings *my_settings, QSettings *default_stgs)
{
	QSettings *settings;
	if (my_settings != NULL)
		settings = my_settings;
	else if (default_stgs != NULL)
		settings = default_stgs;
	else
		return;

	settings->beginGroup("logWindow_params");

	//положение окна
	if(settings->contains("position"))
		move(settings->value("position", QPoint(100, 200)).toPoint());

	if(settings->contains("size"))
		resize(settings->value("size", QSize(615, 260)).toSize());
/*
	if(settings->contains("last_visible"))
		setVisible(settings->value("last_visible", false).toBool());
*/
	//дефолтные пути
	default_load_path = settings->value("load_path", "").toString();
	default_save_path = settings->value("save_path", "").toString();

	settings->endGroup();
}