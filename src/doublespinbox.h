#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QWidget>
#include <QString>
#include <QAbstractSpinBox>
#include <QValidator>
#include <QLineEdit>
#include <float.h>

class DoubleSpinBox: public QAbstractSpinBox
{
public:
	DoubleSpinBox(QWidget *parent = NULL);
	~DoubleSpinBox();

	double value();
	void setValue(double value);
	void setRange(double minimum, double maximum);
	void setSuffix(QString suffix);
	virtual void stepBy(int steps);

private:
	mutable double val;
	double min;
	double max;
	QString suffix;

protected:
	double valueFromText ( const QString & text ) const;
	QString textFromValue ( double value ) const;
	QValidator::State validate ( QString & text, int & pos ) const;
	QAbstractSpinBox::StepEnabled stepEnabled() const;
	void fixup ( QString & input ) const;
};

#endif //DOUBLESPINBOX_H
