#ifndef GRAPHTIMEOFFSET_H
#define GRAPHTIMEOFFSET_H


/*
Этот класс используют ChartDlg, ScaleEngine, ScaleDraw, GraphPicker. Сейчас каждый из них наследует this, 
т.е. имеет собственную структуру TOffset в своей области памяти. Логичнее возложить эти обязанности на 
ChartDlg(хранение, addOffset, Clear), а в ScaleEngine, ScaleDraw и GraphPicker передавать указатель на offset.
Только Scale и Picker-классам еще потребуется функция CaclValue(), надо придумать как не определять ее в каждом классе,
и при этом не наследоваться от this.
*/

//в списке этой структуры хранятся смещения (величина и начало разрывов)
typedef struct TOffset{
	TOffset *next;
	double timestamp;
	double offset;
} TOffset;

/*
в этом списке хранятся тики, поправленные на целые позиции и попавшие при этом
под непредусмотренный разрыв (это проявляется в появлении нецелых главных штрихов на шкале)
Структуру TTicksVal неявно используют все классы, в которых определен TimeOffset, для
нахождения истинного положения штрихов на шкале
*/
typedef struct TTicksVal{
	TTicksVal *next;
	double calc_value;
	double real_value;
} TTicksVal;

/*
содержит координаты штрихов, около которых ставить метки не следует
*/
typedef struct TDisTicks{
	TDisTicks *next;
	double value;
} TDisTicks;

class TimeOffset
{
public:
	TimeOffset(TOffset *_offset);
	TimeOffset();
	~TimeOffset();


	void addOffset(double t1, double t2);
	double CalcValue(double v) const;
	void Clear();

	void moveData(double time_offset);

	void addRealTick(double calcValue, double realValue);
	void addDisableTicks(double v);
	bool testTick(double v);

	int num_ticks;

	TOffset *offset;
	TTicksVal *ticks;
	TDisTicks *dis_ticks;
};

#endif // GRAPHTIMEOFFSET_H
