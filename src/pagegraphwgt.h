#ifndef PAGEGRAPHWGT_H
#define PAGEGRAPHWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <QIcon>
#include <QPixmap>
#include <QPainter>

#include <graphsettings.h>
#include <pagegraphsetupwgt.h>

namespace Ui {
	class PageGraphWgtClass;
}

//Position
class PageGraphWgt : public QWidget {
    Q_OBJECT
	Q_DISABLE_COPY(PageGraphWgt)
public:
    explicit PageGraphWgt(QWidget *parent, GraphCurveSettings *_gcStgs, QString _group);
    virtual ~PageGraphWgt();

	void FromUiToClass();
	void FromClassToUi();
	void FromUiToSettings(QSettings *settings);
	QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
	void SetCaption(QString caption);
	void HideColor();

	Ui::PageGraphWgtClass *m_ui;
	GraphCurveSettings *gcStgs;
	QString group;
};

#endif // PAGEGRAPHWGT_H
