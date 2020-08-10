#include <ui_nodevicehelperdlg.h>
#include <nodevicehelperdlg.h>
#include <QMessageBox>
#include <QProcess>
#include <QFile>

const char* rulesname = "60-ximc.rules";
const char* etcrulespath = "/etc/udev/rules.d/";
const char* dialout = "dialout";

NoDeviceHelperDlg::NoDeviceHelperDlg(QWidget *parent, QString title)
    : QDialog(parent),
	m_ui(new Ui::NoDeviceHelperDlgClass)
{
	m_ui->setupUi(this);
	setWindowTitle(title);
	Init();
	m_ui->label_2->setOpenExternalLinks(true);
	connect(m_ui->btnFixGroup, SIGNAL(clicked()), this, SLOT(onFixGroupPressed()));
	connect(m_ui->btnFixUdev, SIGNAL(clicked()), this, SLOT(onFixUdevPressed()));
}

NoDeviceHelperDlg::~NoDeviceHelperDlg()
{
}

void NoDeviceHelperDlg::Init()
{
	QProcess who;
	who.start("whoami");
	if (!who.waitForStarted()) {
		return;
	}
	if (!who.waitForFinished()) {
		return;
	}
	QByteArray result = who.readAll();
	this->username = QString(result);

	checkUdevOk();
	checkGroupOk();
}

void NoDeviceHelperDlg::onFixGroupPressed()
{
	QProcess p;
	p.start(QString("pkexec adduser %1 %2").arg(username).arg(dialout));
	p.waitForFinished();
	p.close();
	checkGroupOk();
}

void NoDeviceHelperDlg::onFixUdevPressed()
{
	QFile rulefile(":/ximcrules/rulesfile");
	rulefile.open(QIODevice::ReadOnly);
	size_t size = rulefile.size();
	char* mem = (char*)malloc(size);
	QString tmpfilename = QString("/tmp/%1").arg(rulesname);
	QFile tmpfile(tmpfilename);
	if (mem) {
		rulefile.read(mem, size);
		rulefile.close();
		if (tmpfile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
			size_t written = tmpfile.write(mem, size);
			tmpfile.close();
			if (written == size) {
				QProcess p;
				p.start(QString("pkexec cp %1 %2").arg(tmpfilename).arg(etcrulespath));
				p.waitForFinished();
				p.close();
				checkUdevOk();
			}
			tmpfile.remove();
		}
		free(mem);
	}
}

void NoDeviceHelperDlg::checkUdevOk()
{
	// Check if udev rules OK
	QFile rules(QString("%1%2").arg(etcrulespath).arg(rulesname));
	if (rules.exists()) {
		setUdevOk(true);
	}
}

void NoDeviceHelperDlg::checkGroupOk()
{
	// Check if group membership OK (needs relog to activate)
	QProcess p;
	p.start(QString("id -nG %1").arg(username));
	p.waitForFinished();
	p.close();
	QByteArray res = p.readAll();
	QString groups(res);
	QRegExp rx(QString("\\W%1\\W").arg(dialout));
	if (rx.pos(0) != -1) {
		setGroupOk(true);
	}
}

void NoDeviceHelperDlg::setUdevOk(bool is_ok)
{
	m_ui->btnFixUdev->setDisabled(is_ok);
	QString text;
	if (is_ok) {
		text = "Udev check OK: udev file is already present";
	}
	m_ui->isUdevOkLabel->setText(text);
}

void NoDeviceHelperDlg::setGroupOk(bool is_ok)
{
	m_ui->btnFixGroup->setDisabled(is_ok);
	QString text;
	if (is_ok) {
		text = "Group check OK: current user is in 'dialout' group";
	}
	m_ui->isGroupOkLabel->setText(text);

}

void NoDeviceHelperDlg::enableLinux()
{
	// m_ui->groupBox_3->hide();
}

void NoDeviceHelperDlg::enableWin()
{
	m_ui->groupBox_2->hide();
	m_ui->groupBox->hide();
}

void NoDeviceHelperDlg::enableApple()
{
	m_ui->groupBox_2->hide();
	m_ui->groupBox->hide();
}