#define _MATH_DEFINES_DEFINED // math.h conflict with qwt
#include <math.h>
#include <floatspinbox.h>

FloatSpinBox::FloatSpinBox(QWidget *parent):QAbstractSpinBox(parent)
{
	setRange(-FLT_MAX, FLT_MAX);
	setValue(0);
	setSuffix(QString());
}

FloatSpinBox::~FloatSpinBox()
{
}

float FloatSpinBox::value()
{
	return this->val;
}

void FloatSpinBox::setValue(float value)
{
	this->val = value;
	this->lineEdit()->setText(textFromValue(val));
}

void FloatSpinBox::setRange(float minimum, float maximum)
{
	this->min = minimum;
	this->max = maximum;
}

void FloatSpinBox::setSuffix(QString _suffix)
{
	float value = valueFromText(this->text()); // save value (from text with old suffix)
	this->suffix = _suffix; // update suffix
	this->setValue(value); // set old value with new suffix (see setValue)
}

QValidator::State FloatSpinBox::validate ( QString & text, int & pos ) const
{
	Q_UNUSED(pos)
	bool ok;
	QString t = text;
	t.chop(suffix.length());
	float tryconv = QLocale::system().toFloat(t, &ok);
	QValidator::State state;
	if (ok)
		if (textFromValue(tryconv) == text)
			state = QValidator::Acceptable;
		else
			state = QValidator::Intermediate;
	else
		state = QValidator::Invalid;
	if (state == QValidator::Acceptable)
		val = tryconv;
	return state;
}

float FloatSpinBox::valueFromText ( const QString & text ) const
{
	QString t = text;
	t.chop(suffix.length());
	return t.toFloat();
}

QString FloatSpinBox::textFromValue ( float value ) const
{
	QString s;
	int decim = -log10(fabs(value)) + 7; // because 1/2^23
	if (decim < 0)
		decim = 0;

	s = QLocale::system().toString(value, 'f', decim);
	// manual chop because I think it's faster than QRegExp
	while (s.endsWith("0") && s.contains(QLocale::system().decimalPoint()))
		s.chop(1);
	if (s.endsWith(QLocale::system().decimalPoint()))
		s.chop(1);
	return s + suffix;
}

void FloatSpinBox::stepBy(int steps)
{
	if (!this->isReadOnly())
		setValue(val + steps);
}

QAbstractSpinBox::StepEnabled FloatSpinBox::stepEnabled() const
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

void FloatSpinBox::fixup ( QString & input ) const
{
	input = textFromValue(valueFromText(input));
}
