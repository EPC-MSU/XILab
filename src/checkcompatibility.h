#ifndef CHECKCOMPATIBILITY_H
#define CHECKCOMPATIBILITY_H

// #include <QDialog>
// #include <QTreeWidget>
// #include <QStandardItem>
// #include <QDebug>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QDateTime>
#include <tinyxml2.h>
using namespace tinyxml2;

#include "version_struct.h"


typedef struct 
{
	version_t xilab;
	version_t firmware;
} DataChek;

void search_max_allowed(XMLElement* root_element, leaf_element_t *max_allowed, hardware_version_t hw_ver);
void MessageCompatibility(QString message);
void  chek();



//void MessageCompatibility(QString message);


#endif // CHECKCOMPATIBILITY_H