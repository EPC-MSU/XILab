#ifndef LOGSETTINGS_H
#define LOGSETTINGS_H

#include <QDir>

class LogSettings {
public:
	LogSettings(int xll_index, int sll_index, int lll_index, bool autosave_enabled, QDir autosave_path);
	LogSettings();
    ~LogSettings();

	int xll_index;
	int sll_index;
	int lll_index;
	bool autosave_enabled;
	QDir autosave_path;
};

#endif // LOGSETTINGS_H
