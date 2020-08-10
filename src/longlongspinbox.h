#ifndef LONGLONGSPINBOX_H
#define LONGLONGSPINBOX_H

#include <QWidget>
#include <QString>
#include <QAbstractSpinBox>
#include <QValidator>
#include <QLineEdit>

class LongLongSpinBox: public QAbstractSpinBox
{
public:
	LongLongSpinBox(QWidget *parent = NULL);
	~LongLongSpinBox();

	long long value();
	void setValue(long long value);
	void setRange(long long minimum, long long maximum);
	virtual void stepBy(int steps);

private:
	mutable long long val;
	long long min;
	long long max;

protected:
	long long valueFromText ( const QString & text ) const;
	QString textFromValue ( long long value ) const;
	QValidator::State validate ( QString & text, int & pos ) const;
	QAbstractSpinBox::StepEnabled stepEnabled() const;
};

#endif //LONGLONGSPINBOX_H
