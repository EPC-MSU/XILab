#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#define _MATH_DEFINES_DEFINED // math.h conflict with qwt
#include <math.h>
#include <stepspinbox.h>
#include <ximc.h>
#include <functions.h>

#define DEFAULT_PRECISION 6

// Calls setShowStyle with params on every StepSpinBoxItem with matching serial
void CounterStepSpinBoxSetter::setShowStyleAll(unsigned int sn, bool permanent, UserUnitSettings old_uuStgs, UserUnitSettings new_uuStgs, multiplier_t old_passed_mult, multiplier_t new_passed_mult) {
	StepSpinBoxItem::ShowStyle st;
	std::list<Counter<StepSpinBoxItem>*>::iterator i;
	for (i = instances.begin(); i != instances.end(); ++i) {
		StepSpinBoxItem* item = dynamic_cast<StepSpinBoxItem*>(*i);
		if (sn == item->getSerial()) {
			// Deciding part
			if (new_uuStgs.enable && item->getFloatStyleAllowed()) {
				st = StepSpinBoxItem::STYLE_FLOAT;
			} else if (isSpeedUnitRotation(new_passed_mult.fbtype)) {
				st = StepSpinBoxItem::STYLE_INT;
			} else { // fbtype == FEEDBACK_NONE or default behaviour
				st = StepSpinBoxItem::STYLE_FRAC;
			}
			if (permanent && item->getTempChangeAllowed()) // Avoids setting temporary controls twice (todo: check if it's really ok)
				continue;

			// Setting part
			item->setShowStyle(st, old_uuStgs, new_uuStgs, old_passed_mult, new_passed_mult, permanent);
		}
	}
}
// Calls setStepFactor with params on every StepSpinBoxItem with matching serial
void CounterStepSpinBoxSetter::setStepFactorAll(unsigned int sn, bool permanent, unsigned int sf) {
	std::list<Counter<StepSpinBoxItem>*>::iterator i;
	for (i = instances.begin(); i != instances.end(); ++i) {
		StepSpinBoxItem* item = dynamic_cast<StepSpinBoxItem*>(*i);
		if (sn == item->getSerial()) {
			if (!permanent && !item->getTempChangeAllowed())
				continue;
			else
				item->setStepFactor(sf);
		}
	}
}


StepSpinBoxItem::StepSpinBoxItem(QWidget *parent):QDoubleSpinBox(parent)
{
	show_style = STYLE_NONE; // default show style
	precision = 0; // goes with STYLE_INT
	setUnitType(UserUnitSettings::TYPE_UNKNOWN); // default unit type
	setStepFactor(256); // default step factor
	setMaximum(INT_MAX, 255); // should go after step factor
	setMinimum(INT_MIN, -255); // this too

	setDecimals(9); // Important, because we might want to load double (such as slider value) from settings into a frac-style spinbox on startup and lower decimal values will truncate it
	QDoubleSpinBox::setValue(0);

	float_style_allowed = true; // almost all controls are in settings window, but now they must be recalculated
	temp_change_allowed = true; // same as above; can make temporary changes which are allowed for settings, but not for main window

	basemult = 1;
	feedbacktype = 0;
}

StepSpinBoxItem::~StepSpinBoxItem()
{
}

static double roundWithPrecision(double v, int precision)
{
	double dec_precision = pow(10.0F, precision);
	int64_t temp = ceil(v * dec_precision);
	return (double)(temp) / dec_precision;
}

void StepSpinBoxItem::setMinimum(int step, int ustep)
{
	min_value = (double)step + (double)ustep/step_factor;
	QDoubleSpinBox::setMinimum(min_value);
}

void StepSpinBoxItem::setMaximum(int step, int ustep)
{
	max_value = (double)step + (double)ustep/step_factor;
	QDoubleSpinBox::setMaximum(max_value);
}

void StepSpinBoxItem::setShowStyle(StepSpinBoxItem::ShowStyle new_show_style, UserUnitSettings old_unitStgs, UserUnitSettings new_unitStgs, multiplier_t old_multi, multiplier_t new_multi, bool permanent)
{
	QString suffix;
	int precision = 9; // default precision
	double feedback_multiplier = 1, userunit_multiplier = 1, rpm_multiplier = 1;
	if (!getTempChangeAllowed() && !permanent)
		return;

	if (getFloatStyleAllowed()) {
		suffix = new_unitStgs.getSuffix(unit_type, new_multi.fbtype);
		if (new_unitStgs.enable) {
			precision = new_unitStgs.precision;
			userunit_multiplier *= new_unitStgs.getUnitPerStep();
		}
		if (old_unitStgs.enable) {
			userunit_multiplier /= old_unitStgs.getUnitPerStep();
		}
	} else { // only one field matches this now - the userunits stepspinbox
		suffix = new_unitStgs.getOffSuffix(unit_type, new_multi.fbtype);
	}

	if (unit_type == UserUnitSettings::TYPE_COORD) { // counts = steps * (counts per turn) / (steps per turn)
		feedback_multiplier = new_multi.counts_per_turn / new_multi.steps_per_turn;
	} else { // rpm = 60 * rps = 60 * turns/s = 60 * (1/s) / (1/turn) = 60 * (steps/s) / (steps/turn) = steps/s * (60 / steps per turn)
		feedback_multiplier = 60 / new_multi.steps_per_turn;
		if (isSpeedUnitRotation(new_multi.fbtype) || isSpeedUnitRotation(old_multi.fbtype)) { // for rpm and rpm/s we add a special conversion multiplier
			rpm_multiplier = 60 / new_multi.counts_per_turn;
		}
	}

	this->precision = precision;
	this->setDecimals(precision);

	double saved_value = valueFromText(this->text());
	if (!suffix.isEmpty() && (this->suffix() != suffix)) { // testing for empty suffix is done in case setShowStyle was called in its reduced form
		setSuffix(suffix);
	}

	// Initial setup
	if (this->show_style == STYLE_NONE) {
		basemult = 1;
		if (!getFloatStyleAllowed()) { // A hack for the step field on userunits page which otherwise wrongly initializes to zero
			saved_value = basevalue();
		}
	}

	// Switch between none and encoder feedback (enc.feedback is only for stepper at the moment) with uu disabled
	if (new_show_style == STYLE_FRAC && this->show_style == STYLE_INT) {
		saved_value /= feedback_multiplier;
	}
	if (new_show_style == STYLE_INT && this->show_style == STYLE_FRAC) {
		saved_value *= feedback_multiplier;
	}

	// Switch between uu enabled and disabled
	if (new_show_style == STYLE_FLOAT && this->show_style != STYLE_FLOAT) {
		saved_value *= userunit_multiplier / rpm_multiplier;
		basemult = new_unitStgs.getUnitPerStep() / rpm_multiplier;

		if (permanent && !getTempChangeAllowed()) {	
			// special case of moveto/shifton permanent change where userunits changed type earlier in non-permanent-type call because of feedback change
			// that is, we want to recalc X steps into Y mm, but userunits are already switched to Z counts per mm, instead of Z steps per mm
			if ((this->show_style == STYLE_FRAC && isSpeedUnitRotation(new_multi.fbtype)) ||
			    (this->show_style == STYLE_INT && (new_multi.fbtype == FEEDBACK_NONE || new_multi.fbtype == FEEDBACK_NONE))) {
				//saved_value *= feedback_multiplier;
				//basemult *= feedback_multiplier;
				if (!new_unitStgs.enable)
				{
					saved_value *= feedback_multiplier;
					basemult *= feedback_multiplier;
				}
			}
		}
	}
	if (new_show_style != STYLE_FLOAT && this->show_style == STYLE_FLOAT) {
		if (permanent && !getTempChangeAllowed()) {
			// special case, part 2: conversion from userunits back to steps or counts
			saved_value /= new_unitStgs.getUnitPerStep() * rpm_multiplier; // actually we don't care about rpm, because the only such controls are TYPE_COORD
		} else {
			saved_value *= userunit_multiplier * rpm_multiplier;
		}
		basemult = 1;
	}

	// EITHER changed uu multiplier/precision OR recalc on changed feedback with uu enabled
	if (new_show_style == STYLE_FLOAT && this->show_style == STYLE_FLOAT) {
		if (new_multi.fbtype == old_multi.fbtype) { // changed uu multiplier/precision
			if (this->feedbacktype == new_multi.fbtype) {
				saved_value *= userunit_multiplier;
				basemult *= userunit_multiplier;
			}
			else{
				saved_value *= userunit_multiplier;
				basemult *= userunit_multiplier;
			}
		} else { // recalc on changed feedback with uu enabled
			if (isSpeedUnitRotation(new_multi.fbtype) && (old_multi.fbtype == FEEDBACK_NONE || old_multi.fbtype == FEEDBACK_EMF)) {
				basemult /= feedback_multiplier;
			}
			if ((new_multi.fbtype == FEEDBACK_NONE || new_multi.fbtype == FEEDBACK_EMF) && isSpeedUnitRotation(old_multi.fbtype)) {
				basemult *= feedback_multiplier;
			}
		}
	}

	if (permanent ^ getTempChangeAllowed()) {
		this->feedbacktype = new_multi.fbtype;
	}
	this->show_style = new_show_style;
	this->setValue(saved_value);

	this->update();
}

void StepSpinBoxItem::setStep(int step)
{
	setValue(step, getUStep());
}

void StepSpinBoxItem::setUStep(int ustep)
{
	setValue(getStep(), ustep);
}

void StepSpinBoxItem::setValue(int step, int ustep)
{
	QDoubleSpinBox::setValue((double)step + (double)ustep/step_factor);
}

void StepSpinBoxItem::setBaseValue(int step, int ustep)
{
	setBaseValue((double)step + (double)ustep/step_factor);
}

void StepSpinBoxItem::setValue(double value)
{
	QDoubleSpinBox::setValue(value);
}

int StepSpinBoxItem::getStep() const
{
	return ((int)(value()));
}

int StepSpinBoxItem::getUStep() const
{
	return ((value() - getStep())*step_factor);
}

int StepSpinBoxItem::getBaseStep() const
{
	return ((int)round(basevalue()));
}

int StepSpinBoxItem::getBaseUStep() const
{
	return round((basevalue() - getBaseStep())*step_factor);
}

unsigned int StepSpinBoxItem::UgetBaseStep() const
{
	return ((unsigned int)trunc(basevalue()));
}

unsigned int StepSpinBoxItem::UgetBaseUStep() const
{
	return round((basevalue() - UgetBaseStep())*step_factor);
}

double StepSpinBoxItem::baseMultiplier() const
{
	return basemult;
}

double StepSpinBoxItem::basevalue() const
{
	return QDoubleSpinBox::value() / baseMultiplier();
}

int StepSpinBoxItem::basevalueInt() const
{
	return round(this->basevalue());
}

void StepSpinBoxItem::setBaseValue(double value)
{
	QDoubleSpinBox::setValue(roundWithPrecision(value * baseMultiplier(), DEFAULT_PRECISION));
}

void StepSpinBoxItem::setStepFactor(unsigned int sf)
{
	step_factor = sf;

	setValue((value()), round((value() - getStep())*step_factor));
}

QValidator::State StepSpinBoxItem::validate ( QString & text, int & pos ) const
{
	QRegExp rx;
	if (show_style == STYLE_FRAC){
		rx.setPattern("([-]?[\\d]*)\\s([\\d]{,3})" + getStepFactorStr() + suffix());
	}
	else if (show_style == STYLE_INT){
		rx.setPattern("[-]?[\\d]*" + suffix());
	}
	else if (show_style == STYLE_FLOAT) {
		rx.setPattern("[-]?[\\d]+[\\.]?[\\d]{0," + QString::number(precision) + "}" + suffix());
	}

	double value = valueFromText(text);
	if (value < minimum()) {
		return QValidator::Invalid;
	}
	else if (value > maximum()) {
		return QValidator::Invalid;
	}

	QRegExpValidator v(rx, NULL);
	return v.validate(text, pos);
}

double StepSpinBoxItem::valueFromText ( const QString & text ) const
{
	double ret_value = 0;
	if (show_style == STYLE_FRAC) {
		int step = 0;
		int ustep = 0;
		QRegExp rx("([-])?([\\d]*)\\s([\\d]{,3})" + getStepFactorStr() + suffix());
		rx.indexIn(text);
		QStringList list = rx.capturedTexts();
		if(list.size() == 4){
			step = list[2].toInt();
			ustep = list[3].toInt();
			if(list[1] == "-"){
				step = -abs(step);
				ustep = -abs(ustep);
			}
		}
		ret_value = step + (double)ustep/step_factor;
	}
	else if (show_style == STYLE_INT || show_style == STYLE_FLOAT) {
		QString temp = text;
		temp.chop(suffix().length());
		ret_value = temp.toDouble();
	}

	if (ret_value < minimum())
		ret_value = min_value;
	else if (ret_value > maximum())
		ret_value = max_value;

	return ret_value;
}

QString StepSpinBoxItem::textFromValue ( double value ) const
{
	int tempstep = this->getStep();
	int tempustep = this->getUStep();
	QString s;

	if(show_style == STYLE_FRAC) {
		s = tr("%1 %2").arg(tempstep).arg(abs(tempustep))+getStepFactorStr();
		if(value < 0 && tempstep == 0 && tempustep != 0)
			s = "-" + s;
	}
	else if (show_style == STYLE_FLOAT)
		s = QString::number(value, 'f', precision);
	else { // show_style == STYLE_INT
		s = QString("%1").arg(tempstep);
	}
	return s;
}
