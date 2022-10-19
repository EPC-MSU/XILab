#include <math.h>
#include <longlongspinbox.h>

LongLongSpinBox::LongLongSpinBox(QWidget *parent):QAbstractSpinBox(parent)
{
	setRange(LLONG_MIN, LLONG_MAX);
	setValue(0);
}

LongLongSpinBox::~LongLongSpinBox()
{
}

long long LongLongSpinBox::value()
{
	return this->val;
}

void LongLongSpinBox::setValue(long long value)
{
	this->val = value;
	this->lineEdit()->setText(textFromValue(val));
}

void LongLongSpinBox::setRange(long long minimum, long long maximum)
{
	this->min = minimum;
	this->max = maximum;
}

QValidator::State LongLongSpinBox::validate ( QString & text, int & pos ) const
{
	Q_UNUSED(pos)
	if (text.isEmpty() || text == "-")
		return QValidator::Intermediate;
	long long fromtext = valueFromText(text);
	if (fromtext < min || fromtext > max) {
		return QValidator::Invalid;
	}

	bool ok;
	long long tryconv = text.toLongLong(&ok);
	QValidator::State state;
	if (ok)
		state = QValidator::Acceptable;
	else
		state = QValidator::Invalid;
	if (state == QValidator::Acceptable)
		val = tryconv;
	return state;
}

long long LongLongSpinBox::valueFromText ( const QString & text ) const
{
	return text.toLongLong();
}

QString LongLongSpinBox::textFromValue ( long long value ) const
{
	QString s;
	s.setNum(value);
	return s;
}

void LongLongSpinBox::stepBy(int steps)
{
	setValue(val + steps);
}

QAbstractSpinBox::StepEnabled LongLongSpinBox::stepEnabled() const
{
	if (wrapping())
		return QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled;

	if (val <= min)
		return QAbstractSpinBox::StepUpEnabled;
	else if (val >= max)
		return QAbstractSpinBox::StepDownEnabled;
	else {
		StepEnabled step;
		step = QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled;
		return step;
	}
}
