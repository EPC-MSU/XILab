#ifndef GRAPHCOMMONVARS_H
#define GRAPHCOMMONVARS_H

#define PLOT_HEIGHT 130
#define PLOTSNUM	19
#define DIGNUM		9

#define POSNUM		0
#define SPEEDNUM	1
#define ENCODNUM	2
#define ENVOLTNUM	3
#define	ENCURRNUM	4
#define PWRVOLTNUM	5
#define PWRCURRNUM	6
#define USBVOLTNUM	7
#define USBCURRNUM	8
#define	VOLTANUM	9
#define	VOLTBNUM	10
#define	VOLTCNUM	11
#define	CURRANUM	12
#define	CURRBNUM	13
#define	CURRCNUM	14
#define PWMNUM		15
#define TEMPNUM		16
#define JOYNUM		17
#define POTNUM		18


#if defined(WIN32) || defined(WIN64) || defined(__LINUX__)
#define LEFT_EXTENT     50
#endif

#ifdef __APPLE__
#define LEFT_EXTENT     30
#endif

#endif // GRAPHCOMMONVARS_H
