#ifndef CHOOSEFIRMWARE_H
#define CHOOSEFIRMWARE_H

#include <QDialog>
#include <QTreeWidget>
#include <QStandardItem>
#include <QDebug>
#include <QUrl>
#include <QDateTime>
#include <tinyxml2.h>
#include "version_struct.h"

namespace Ui {
class ChooseFirmware;
}

class ChooseFirmware : public QDialog
{
    Q_OBJECT
    //Q_DISABLE_COPY(ChooseFirmware)

public:
    explicit ChooseFirmware(QString xml_url, QWidget *parent = 0);
    ~ChooseFirmware();
	void fill_treeWdg(tinyxml2::XMLElement* root_element, leaf_element_t* max_allowed, hardware_version_t hw_ver);

public slots:
    void checkClk();
	void get_double_clk_item(QTreeWidgetItem* Item, int col);
	void up_list();
	void showMessageAndClose(QString, bool, int dummy=0); // first parameter is the message to show, second parameter decides whether we should close, third parameter is a dummy
signals:
	void clicked(QString, QString);
	void updateList(QUrl);
private:
    Ui::ChooseFirmware *ui;
	QString xml_url;
};

#endif // CHOOSEFIRMWARE_H
