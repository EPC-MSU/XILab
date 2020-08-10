#ifndef FLOATSPINBOX_H
#define FLOATSPINBOX_H

#include <QWidget>
#include <QString>
#include <QAbstractSpinBox>
#include <QValidator>
#include <QLineEdit>
#include <float.h>

class FloatSpinBox: public QAbstractSpinBox
{
public:
	FloatSpinBox(QWidget *parent = NULL);
	~FloatSpinBox();

	float value();
	void setValue(float value);
	void setRange(float minimum, float maximum);
	void setSuffix(QString suffix);
	virtual void stepBy(int steps);

private:
	mutable float val;
	float min;
	float max;
	QString suffix;

protected:
	float valueFromText ( const QString & text ) const;
	QString textFromValue ( float value ) const;
	QValidator::State validate ( QString & text, int & pos ) const;
	QAbstractSpinBox::StepEnabled stepEnabled() const;
	void fixup ( QString & input ) const;
};

#endif //FLOATSPINBOX_H
