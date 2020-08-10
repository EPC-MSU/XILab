#ifndef CYCLICSETTINGS_H
#define CYCLICSETTINGS_H

class CyclicSettings {
public:
	enum UnitType{CYCLICUNKNOWN, CYCLICBTB, CYCLICPTP_FINE};
	CyclicSettings();
    ~CyclicSettings();

	UnitType GetType(){return cyclic_type;};
	double GetLeft(){return left;};
	double GetRight(){return right;};
	
	void SetType(UnitType _cyclic_type){cyclic_type = _cyclic_type;};
	void SetLeft(double _left){left = _left;};
	void SetRight(double _right){right = _right;};
private:
	UnitType cyclic_type;
	double left;
	double right;
};

#endif // CYCLICSETTINGS_H
