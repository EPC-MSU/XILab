#ifndef XSETTINGS_H
#define XSETTINGS_H

#include <QSettings>

class XSettings :
	public QSettings
{
public:
	XSettings( const QString & fileName, Format format, QIODevice::OpenMode openMode );
	~XSettings(void);
};

#endif // XSETTINGS_H
