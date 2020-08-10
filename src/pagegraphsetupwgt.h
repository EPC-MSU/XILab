#ifndef PAGEGRAPHSETUPWGT_H
#define PAGEGRAPHSETUPWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

#include <graphsettings.h>
#include <colorbutton.h>

void CurveInitUI(QComboBox *linestyleBox);
void CurveGetSettings(QSettings *settings, QString groupname,
					  QDoubleSpinBox *linewidthValue,
					  QDoubleSpinBox *minScaleValue,
					  QDoubleSpinBox *maxScaleValue,
					  QCheckBox *antialiasingChk,
					  QCheckBox *autoscaleChk,
					  ColorButton *colorBox, 
 					  QComboBox *linestyleBox);
QString CurveSetSettings(QString groupname, 
						 QDoubleSpinBox *linewidthValue,
						 QDoubleSpinBox *minScaleValue,
						 QDoubleSpinBox *maxScaleValue,
						 QCheckBox *antialiasingChk,
						 QCheckBox *autoscaleChk,
						 ColorButton *colorBox, 
						 QComboBox *linestyleBox, 
						 QSettings *settings,
						 QSettings *default_stgs = NULL);

void CurveUpdatePage(GraphCurveSettings *gcurveStgs, 
					 QDoubleSpinBox *linewidthValue,
					 QDoubleSpinBox *minScaleValue,
					 QDoubleSpinBox *maxScaleValue,
					 QCheckBox *antialiasingChk,
					 QCheckBox *autoscaleChk,
					 ColorButton *colorBox, 
					 QComboBox *linestyleBox);
void CurveGetData(GraphCurveSettings *gcurveStgs, 
				  QDoubleSpinBox *linewidthValue,
				  QDoubleSpinBox *minScaleValue,
				  QDoubleSpinBox *maxScaleValue,
				  QCheckBox *antialiasingChk,
				  QCheckBox *autoscaleChk,
				  ColorButton *colorBox, 
				  QComboBox *linestyleBox);

#endif // PAGEGRAPHSETUPWGT_H
