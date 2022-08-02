#ifndef CUSTOMSPINBOX_H
#define CUSTOMSPINBOX_H

#include <QWidget>
#include <QString>
#include <QDoubleSpinBox>
#include <QValidator>
#include <userunitsettings.h>

typedef struct multiplier_t
{
	double steps_per_turn;
	double counts_per_turn;
	int fbtype;
} multiplier_t;

class StepSpinBoxItem: public QDoubleSpinBox
{
	friend class CounterStepSpinBoxSetter;
public:
	StepSpinBoxItem(QWidget *parent = NULL);
	virtual ~StepSpinBoxItem();

	void setStep(int step);
	void setUStep(int ustep);
	void setValue(int step, int ustep=0);
	void setBaseValue(int step, int ustep);
	void setValue(double val);
	int getStep() const;
	int getUStep() const;
	int getBaseStep() const;
	int getBaseUStep() const;
	unsigned int UgetBaseStep() const;
	unsigned int UgetBaseUStep() const;
	double baseMultiplier() const;
	double basevalue() const;
	int basevalueInt() const;
	void setBaseValue(double val);
	void setStepFactor(unsigned int sf);
	unsigned int getStepFactor() const {return step_factor;}
	enum ShowStyle{STYLE_NONE, STYLE_INT, STYLE_FRAC, STYLE_FLOAT};
	void setShowStyle(StepSpinBoxItem::ShowStyle st, UserUnitSettings old_uStgs, UserUnitSettings new_uStgs, multiplier_t old_multi, multiplier_t new_multi, bool permanent);

	void setMinimum(int step, int ustep=0);
	void setMaximum(int step, int ustep=0);
	bool getFloatStyleAllowed(void) { return float_style_allowed; }
	void setFloatStyleAllowed(bool set) { float_style_allowed = set; }
	bool getTempChangeAllowed(void) { return temp_change_allowed; }
	void setTempChangeAllowed(bool set) { temp_change_allowed = set; }
	bool getUnitType(void) { return unit_type; }
	void setUnitType(UserUnitSettings::UnitType set) { unit_type = set; }
	unsigned int getSerial(void) { return serial; }
	void setSerial(unsigned int set) { serial = set; }
private:
	mutable double min_value;
	mutable double max_value;
	unsigned int step_factor;
	unsigned int precision;
	bool float_style_allowed;
	bool temp_change_allowed;
	unsigned int serial;
	double basemult;
	int feedbacktype;

	QString getStepFactorStr() const {return QString("/%1").arg(step_factor);}
protected:
	double valueFromText ( const QString & text ) const;
	QString textFromValue ( double value ) const;
	QValidator::State validate ( QString & text, int & pos ) const;

	ShowStyle show_style;
	UserUnitSettings::UnitType unit_type;
};

template <typename T> class Counter
{
    static unsigned class_count;
protected:
	static std::list<Counter*> instances;
public:
	Counter() {
		++class_count;
		instances.push_back(this);
	}
	Counter(Counter const&) {
		++class_count;
		instances.push_back(this);
	}
	virtual ~Counter() {
		--class_count;
		typename std::list<Counter*>::iterator i = instances.begin();
		while (i != instances.end())
			if ( this == (*i) )
				i = instances.erase(i);
			else
				++i;
	}
	static unsigned count() {
		return class_count;
	}
};
template <typename T> unsigned Counter<T>::class_count = 0;
template <typename T> std::list<Counter<T>*> Counter<T>::instances;

class CounterStepSpinBoxSetter : public Counter<StepSpinBoxItem>
{
public:
	static void setShowStyleAll(unsigned int sn, bool permanent, UserUnitSettings old_uuStgs, UserUnitSettings new_uuStgs, multiplier_t old_passed_mult, multiplier_t new_passed_mult);
	static void setStepFactorAll(unsigned int sn, bool permanent, unsigned int sf);
};

class StepSpinBox: public StepSpinBoxItem, public CounterStepSpinBoxSetter
{
public:
	StepSpinBox(QWidget* parent = NULL)
	{
		Q_UNUSED(parent)
		setSerial(0);
	}
};

#endif //CUSTOMSPINBOX_H
