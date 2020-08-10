#ifndef VERSION_STRUCT_H
#define VERSION_STRUCT_H

#include <QUrl>

typedef struct
{
	unsigned int major;
	unsigned int minor;
	unsigned int release;
} version_t;

typedef version_t firmware_version_t;
typedef version_t bootloader_version_t;
typedef version_t hardware_version_t;

struct VersionRange {
	version_t start;
	version_t end;
};

typedef struct
{
	bool valid;
	version_t version;
	QString SHA1_string;
	QDateTime UTCTime;
	QUrl URL;
} leaf_element_t;

#endif //VERSION_STRUCT_H
