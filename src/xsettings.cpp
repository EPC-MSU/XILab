#include "xsettings.h"
#include "loggedfile.h"

XSettings::XSettings( const QString & fileName, Format format, QIODevice::OpenMode openMode):QSettings( fileName, format )
{
	LoggedFile test(fileName);
	if (test.open(openMode)) // if opened ok with selected openMode
		test.close(); // then close, because it was only a test
}

XSettings::~XSettings()
{
}
