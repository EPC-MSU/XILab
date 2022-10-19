#include <math.h>
#include <doublespinbox.h>

DoubleSpinBox::DoubleSpinBox(QWidget *parent):QAbstractSpinBox(parent)
{
	setRange(-DBL_MAX, DBL_MAX);
	setValue(0);
	setSuffix(QString());
}

DoubleSpinBox::~DoubleSpinBox()
{
}

double DoubleSpinBox::value()
{
	return this->val;
}

void DoubleSpinBox::setValue(double value)
{
	this->val = value;
	this->lineEdit()->setText(textFromValue(val));
}

void DoubleSpinBox::setRange(double minimum, double maximum)
{
	this->min = minimum;
	this->max = maximum;
}

void DoubleSpinBox::setSuffix(QString _suffix)
{
	double value = valueFromText(this->text()); // save value (from text with old suffix)
	this->suffix = _suffix; // update suffix
	this->setValue(value); // set old value with new suffix (see setValue)
}

QValidator::State DoubleSpinBox::validate ( QString & text, int & pos ) const
{
	Q_UNUSED(pos)
	bool ok;
	QString t = text;
	t.chop(suffix.length());
	double tryconv = QLocale::system().toDouble(t, &ok);
	QValidator::State state;
	if (ok)
		if (textFromValue(tryconv) == text)
			if (tryconv < min || tryconv > max)
				state = QValidator::Intermediate;
			else
				state = QValidator::Acceptable;
		else
			state = QValidator::Intermediate;
	else
		state = QValidator::Invalid;
	if (state == QValidator::Acceptable)
		val = tryconv;

	return state;
}

double DoubleSpinBox::valueFromText ( const QString & text ) const
{
	QString t = text;
	t.chop(suffix.length());
	return t.toDouble();
}

QString DoubleSpinBox::textFromValue ( double value ) const
{
	QString s;
	int decim = -log10(fabs(value)) + 15; // because 1/2^52
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

void DoubleSpinBox::stepBy(int steps)
{
	if (!this->isReadOnly())
		if (val + steps > min && val + steps < max)
			setValue(val + steps);
}

QAbstractSpinBox::StepEnabled DoubleSpinBox::stepEnabled() const
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

void DoubleSpinBox::fixup ( QString & input ) const
{
	double v = valueFromText(input);
	if (v < min || v > max)
		v = val;
	input = textFromValue(v);
}
