#include "attenuator.h"
#include "ui_attenuator.h"
#include <cmath>        // std::abs


Attenuator::Attenuator(AttenSettings& intset,QWidget *parent) :
    QWidget(parent),
	ui(new Ui::Attenuator) 
{
	ui->setupUi(this);
    signalMapper = new QSignalMapper(this);

	WheelPos1 = 0;
    WheelPos2 = 0;

    //two wheels with FILTER_COUNT filters
    groupWheel1 = new QButtonGroup(this);
    groupWheel2 = new QButtonGroup(this);
    vboxWheel[0] = new QHBoxLayout(); 
    vboxWheel[1] = new QHBoxLayout();
    for(int i =0;i != FILTER_COUNT;i++){
        filters[i] = new QPushButton();
        filters[i+FILTER_COUNT] = new QPushButton();
		vboxVWheel[i] = new QVBoxLayout();
        vboxVWheel[i+FILTER_COUNT] = new QVBoxLayout();
        lab_filters[i] = new QLabel(QString::number(intset.wheel1Tran[i]));
		lab_filters[i+FILTER_COUNT] = new QLabel(QString::number(intset.wheel2Tran[i]));
        filters[i]->setFlat(true);
		filters[i+FILTER_COUNT]->setFlat(true);
		signalMapper->setMapping(filters[i], i+1);
        signalMapper->setMapping(filters[i+FILTER_COUNT], i+1+FILTER_COUNT);
		vboxVWheel[i]->addWidget(lab_filters[i],0,Qt::AlignCenter);
		vboxVWheel[i]->addWidget(filters[i]);
        vboxWheel[0]->addLayout(vboxVWheel[i]);
		vboxVWheel[i+FILTER_COUNT]->addWidget(lab_filters[i+FILTER_COUNT],0,Qt::AlignCenter);
		vboxVWheel[i+FILTER_COUNT]->addWidget(filters[i+FILTER_COUNT]);
        vboxWheel[1]->addLayout(vboxVWheel[i+FILTER_COUNT]);
        connect(filters[i],SIGNAL(clicked()),signalMapper,SLOT(map()));
		connect(filters[i+FILTER_COUNT],SIGNAL(clicked()),signalMapper,SLOT(map()));
        groupWheel1->addButton(filters[i]);
		groupWheel2->addButton(filters[i+FILTER_COUNT]);
    }

    groupWheel1->setExclusive(true);
    groupWheel2->setExclusive(true);
    ui->Wheel1->setLayout(vboxWheel[0]);
    ui->Wheel2->setLayout(vboxWheel[1]);
	signalMapper->setMapping(ui->moveBut, FILTER_COUNT*WHEEL_COUNT+1);
    signalMapper->setMapping(ui->resetBut, 0);
	connect(ui->moveBut,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(signalMapper, SIGNAL(mapped(int)),this,SLOT(clicked(int)));
	connect(signalMapper, SIGNAL(mapped(int)),this,SIGNAL(getPosit(int)));
    connect(ui->resetBut,SIGNAL(clicked()),this,SLOT(resetAllBut()));
	connect(ui->moveBut,SIGNAL(clicked()),this,SIGNAL(ChangePosition()));
	connect(ui->atten_calb_button,SIGNAL(clicked()),this,SIGNAL(Calibrate()));
	//change all button on notActive
    resetAllBut();
	connect(ui->wish_transmit_ed,SIGNAL(textChanged(const QString&)),this,SLOT(find_Best(const QString&)));
	if(intset.twoWheels) {
		ui->Wheel2->show();
		use_one_wheel = false;
	}
	else { 
		ui->Wheel2->hide();
		use_one_wheel = true;
	}
	greenPix.load(":/attenuator/images/attenuator/ind_green.png");
	yellowPix.load(":/attenuator/images/attenuator/ind_grey.png");
	redPix.load(":/attenuator/images/attenuator/ind_red.png");
	setCalibrationStatus(Calibration::NO_CALIBRATION);
	should_move = false;
}

void Attenuator::find_Best(const QString & pos_string){
	bool ok = 0;
    double pos = pos_string.toDouble(&ok);
	//enter not number
	if ((!ok)&&(pos_string !="")){
		emit  error_get("Please enter correct transmittance");
		return;
	}
    double razn = 326;//max double precision in IEEE 754-1985
    double whish_transm = pos;
    (!whish_transm) ? whish_transm = -326 : whish_transm = std::log10(pos);
    for(int j = 0;j != FILTER_COUNT; j++){
        double first_wh_fil = lab_filters[j]->text().toDouble();
        (!first_wh_fil) ? first_wh_fil = -326 : first_wh_fil = std::log10(lab_filters[j]->text().toDouble()) ;
        //if user enter zero we find first zero
        if ((first_wh_fil == -326)&&(whish_transm == -326)) {
            WheelPos1 = j+1;//two shift in range [1...8]
            WheelPos2  = j+1+FILTER_COUNT;//two shift in range [9...16]
            clicked(WheelPos1);
            clicked(WheelPos2);
            return;
        }
        for (int i = FILTER_COUNT; i != FILTER_COUNT*WHEEL_COUNT; i++){
            double sec_wh_fil = lab_filters[i]->text().toDouble();
            (!sec_wh_fil) ? sec_wh_fil = -326 : sec_wh_fil = std::log10(lab_filters[i]->text().toDouble());
            //if user enter zero we find first zero
            if ((sec_wh_fil == -326)&&(whish_transm == -326)) {
                WheelPos1 = i+1-FILTER_COUNT;//find first concatenated state
                WheelPos2 = i+1;//
                clicked(WheelPos1);
                clicked(WheelPos2);
                return;
            }
			if(use_one_wheel){
				sec_wh_fil = 0;
			}
            double sum_filters = std::abs(sec_wh_fil + first_wh_fil);
            if(std::abs(sum_filters+whish_transm)<razn){
                razn = std::abs(sum_filters+whish_transm);
                WheelPos1 = j+1;
                WheelPos2 = i+1;
            }
          }
    }
    clicked(WheelPos1);
    clicked(WheelPos2);
}

void Attenuator::clicked(int buttonId){
	if((buttonId != FILTER_COUNT*WHEEL_COUNT+1) || (!buttonId)){
		if(buttonId < FILTER_COUNT+1) {
			//forget previous choosen filter
			resetOneWheel(1);
			//write new choosen filter
			WheelPos1 = buttonId;
		}
		else {
			//forget previous choosen filter
			resetOneWheel(2);
			//write new choosen filter
			WheelPos2 = buttonId;
		} 
		double Pos1Tr;
		(!WheelPos1) ? Pos1Tr = 0 : Pos1Tr = lab_filters[WheelPos1-1]->text().toDouble();
		double Pos2Tr;
		(!WheelPos2) ? Pos2Tr = 0 : Pos2Tr = lab_filters[WheelPos2-1]->text().toDouble();
		double PosSumTr = Pos1Tr * Pos2Tr;
		if(use_one_wheel)
			PosSumTr = Pos1Tr;
		ui->current_transmit_ed->setText(QString::number(PosSumTr));
		//paint choosen filter in green color
		QPixmap pixmap;
		pixmap.load(":/attenuator/images/attenuator/green_circle.png");
		QIcon ButtonIcon(pixmap);
		pixmap = pixmap.scaledToHeight(48);
		filters[buttonId-1]->setIcon(ButtonIcon);
		filters[buttonId-1]->setIconSize(pixmap.size());
		filters[buttonId-1]->setChecked(true);
	}
}

void Attenuator::resetAllBut(){
    //paint all filter in black color
    QPixmap pixmap;
    pixmap.load(":/attenuator/images/attenuator/em_circle.gif");
    QIcon ButtonIcon(pixmap);
    pixmap = pixmap.scaledToHeight(48);
    for (int i =0;i !=FILTER_COUNT*WHEEL_COUNT;i++) {
        filters[i]->setIcon(ButtonIcon);
        filters[i]->setIconSize(pixmap.size());
    }
    //forget about position
    WheelPos1 =0;
    WheelPos2 =0;
	ui->current_transmit_ed->setText("");
}

void Attenuator::setCalibrationStatus(Calibration::Status calb_status) {
	this->calb_status = calb_status;
	setUiCalibrationStatus(calb_status);
}

void Attenuator::setUiCalibrationStatus(Calibration::Status calb_status) {
	switch (calb_status) {
		case Calibration::NO_CALIBRATION:
			ui->atten_calb_label->setPixmap(redPix);
			break;
		case Calibration::IN_PROGRESS:
			ui->atten_calb_label->setPixmap(yellowPix);
			break;
		case Calibration::OK:
			ui->atten_calb_label->setPixmap(greenPix);
			break;
	}
}

void Attenuator::resetOneWheel(int array_num){
    //array = first wheel and array = second wheel
    //forget about choosen filter
    QPixmap pixmap;
    pixmap.load(":/attenuator/images/attenuator/em_circle.gif");
    QIcon ButtonIcon(pixmap);
    pixmap = pixmap.scaledToHeight(48);
    for (int i=FILTER_COUNT*(array_num-1);i !=FILTER_COUNT*array_num;i++) {
        filters[i]->setIcon(ButtonIcon);
        filters[i]->setIconSize(pixmap.size());
		filters[i]->setChecked(false);
        filters[i]->setFlat(true);
    }
}


void Attenuator::get_movePosit(unsigned int& Pos1,unsigned int& Pos2){
	Pos1 = WheelPos1;
    Pos2 = WheelPos2;
}

Attenuator::~Attenuator()
{
    delete ui;
    delete signalMapper;
    delete groupWheel1;
    delete groupWheel2;
	for (int i =0;i != FILTER_COUNT*WHEEL_COUNT;i++) {delete filters[i]; delete lab_filters[i]; delete vboxVWheel[i];}
    for (int i =0;i != WHEEL_COUNT;i++) delete vboxWheel[i];
}
