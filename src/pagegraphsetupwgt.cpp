#include <pagegraphsetupwgt.h>

void CurveInitUI(QComboBox *linestyleBox)
{
	QPixmap pixmap(40, 10);
	//Linestyle select
	linestyleBox->setIconSize(QSize(40, 10));
	QPen pen;
	QPainter paint;
	pen.setWidth(3);
	pen.setColor(QColor(Qt::black));
	paint.begin(&pixmap);

	//Solid
	pixmap.fill(QColor(Qt::white));
	pen.setStyle(Qt::SolidLine);
	paint.setPen(pen);
	paint.drawLine(0, 5, 40, 5);
	linestyleBox->addItem(QIcon(pixmap), "Solid line", Qt::SolidLine);
	//DashLine
	pixmap.fill(QColor(Qt::white));
	pen.setStyle(Qt::DashLine);
	paint.setPen(pen);
	paint.drawLine(0, 5, 40, 5);
	linestyleBox->addItem(QIcon(pixmap), "Dash line", Qt::DashLine);
	//DotLine
	pixmap.fill(QColor(Qt::white));
	pen.setStyle(Qt::DotLine);
	paint.setPen(pen);
	paint.drawLine(0, 5, 40, 5);
	linestyleBox->addItem(QIcon(pixmap), "Dot line", Qt::DotLine);
	//DashDotLine
	pixmap.fill(QColor(Qt::white));
	pen.setStyle(Qt::DashDotLine);
	paint.setPen(pen);
	paint.drawLine(0, 5, 40, 5);
	linestyleBox->addItem(QIcon(pixmap), "Dash dot line", Qt::DashDotLine);
	//DashDotDotLine
	pixmap.fill(QColor(Qt::white));
	pen.setStyle(Qt::DashDotDotLine);
	paint.setPen(pen);
	paint.drawLine(0, 5, 40, 5);
	linestyleBox->addItem(QIcon(pixmap), "Dash dot dot line", Qt::DashDotDotLine);

	paint.end();
}

void CurveGetSettings(QSettings *settings, QString groupname,
					  QDoubleSpinBox *linewidthValue,
					  QDoubleSpinBox *minScaleValue,
					  QDoubleSpinBox *maxScaleValue,
					  QCheckBox *antialiasingChk,
					  QCheckBox *autoscaleChk,
					  ColorButton *colorBox, 
 					  QComboBox *linestyleBox)
{
	settings->beginGroup(groupname);

	settings->setValue("Line_width", linewidthValue->value());
	settings->setValue("Line_color", colorBox->getColor().name());
	settings->setValue("Line_style", linestyleBox->currentIndex());
	settings->setValue("Scale_min", minScaleValue->value());
	settings->setValue("Scale_max", maxScaleValue->value());
	settings->setValue("Autoscale_enable", autoscaleChk->isChecked());
	settings->setValue("Antialiasing_enable", antialiasingChk->isChecked());

	settings->endGroup();
}

QString CurveSetSettings(QString groupname, 
						 QDoubleSpinBox *linewidthValue,
						 QDoubleSpinBox *minScaleValue,
						 QDoubleSpinBox *maxScaleValue,
						 QCheckBox *antialiasingChk,
						 QCheckBox *autoscaleChk,
						 ColorButton *colorBox, 
						 QComboBox *linestyleBox,
						 QSettings *settings, 
						 QSettings *default_stgs)
{
	QString errors;

	settings->beginGroup(groupname);
	if(default_stgs != NULL)
		default_stgs->beginGroup(groupname);

	if(settings->contains("Line_width")){
		double val = settings->value("Line_width", 0).toDouble();
		linewidthValue->setValue(val);
	}
	else if(default_stgs != NULL)
		linewidthValue->setValue(default_stgs->value("Line_width", 0).toDouble());
	else 
		errors+="\"Line_width\" key is not found\n";

	if(settings->contains("Scale_min"))
		minScaleValue->setValue(settings->value("Scale_min", 0).toDouble());
	else if(default_stgs != NULL)
		minScaleValue->setValue(default_stgs->value("Scale_min", 0).toDouble());
	else 
		errors+="\"Scale_min\" key is not found\n";

	if(settings->contains("Scale_max"))
		maxScaleValue->setValue(settings->value("Scale_max", 0).toDouble());
	else if(default_stgs != NULL)
		maxScaleValue->setValue(default_stgs->value("Scale_max", 0).toDouble());
	else 
		errors+="\"Scale_max\" key is not found\n";

	if(settings->contains("Line_color")){
		QColor color(settings->value("Line_color", 0).toString());
		colorBox->setColor(color);
	}
	else if(default_stgs != NULL){
		QColor color(default_stgs->value("Line_color", 0).toString());
		colorBox->setColor(color);
	}
	else 
		errors+="\"Line_color\" key is not found\n";

	if(settings->contains("Line_style"))
		linestyleBox->setCurrentIndex(settings->value("Line_style", 0).toInt());
	else if(default_stgs != NULL)
		linestyleBox->setCurrentIndex(default_stgs->value("Line_style", 0).toInt());
	else 
		errors+="\"Line_style\" key is not found\n";

	if(settings->contains("Autoscale_enable"))
		autoscaleChk->setChecked(settings->value("Autoscale_enable", true).toBool());
	else if(default_stgs != NULL)
		autoscaleChk->setChecked(default_stgs->value("Autoscale_enable", true).toBool());
	else 
		errors+="\"Autoscale_enable\" key is not found\n";

	if(settings->contains("Antialiasing_enable"))
		antialiasingChk->setChecked(settings->value("Antialiasing_enable", true).toBool());
	else if(default_stgs != NULL)
		antialiasingChk->setChecked(default_stgs->value("Antialiasing_enable", true).toBool());
	else 
		errors+="\"Antialiasing_enable\" key is not found\n";

	settings->endGroup();
	if(default_stgs != NULL)
		default_stgs->endGroup();

	return errors;
}

void CurveUpdatePage(GraphCurveSettings *gcurveStgs, 
					 QDoubleSpinBox *linewidthValue,
					 QDoubleSpinBox *minScaleValue,
					 QDoubleSpinBox *maxScaleValue,
					 QCheckBox *antialiasingChk,
					 QCheckBox *autoscaleChk,
					 ColorButton *colorBox, 
					 QComboBox *linestyleBox)
{
	colorBox->setColor(gcurveStgs->pen.color());
	linestyleBox->setCurrentIndex(linestyleBox->findData(gcurveStgs->pen.style()));
	linewidthValue->setValue(gcurveStgs->pen.widthF());
	minScaleValue->setValue(gcurveStgs->minScale);
	maxScaleValue->setValue(gcurveStgs->maxScale);
	antialiasingChk->setChecked(gcurveStgs->antialiasing);
	autoscaleChk->setChecked(gcurveStgs->autoscale);
}

void CurveGetData(GraphCurveSettings *gcurveStgs, 
				  QDoubleSpinBox *linewidthValue,
				  QDoubleSpinBox *minScaleValue,
				  QDoubleSpinBox *maxScaleValue,
				  QCheckBox *antialiasingChk,
				  QCheckBox *autoscaleChk,
				  ColorButton *colorBox, 
				  QComboBox *linestyleBox)
{
	gcurveStgs->pen.setColor(colorBox->getColor());
	gcurveStgs->pen.setStyle((Qt::PenStyle)linestyleBox->itemData(linestyleBox->currentIndex()).toInt());
	gcurveStgs->pen.setWidthF(linewidthValue->value());
	gcurveStgs->minScale = minScaleValue->value();
	gcurveStgs->maxScale = maxScaleValue->value();
	gcurveStgs->antialiasing = antialiasingChk->isChecked();
	gcurveStgs->autoscale = autoscaleChk->isChecked();
}