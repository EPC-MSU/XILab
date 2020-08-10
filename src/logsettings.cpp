#include <logsettings.h>
#include "ximc.h"

LogSettings::LogSettings(int _xll_index, int _sll_index, int _lll_index, bool _autosave_enabled, QDir _autosave_path)
{
	xll_index = _xll_index;
	sll_index = _sll_index;
	lll_index = _lll_index;
	autosave_enabled = _autosave_enabled;
	autosave_path = _autosave_path;
}

LogSettings::LogSettings()
{
	xll_index = LOGLEVEL_WARNING;
	sll_index = LOGLEVEL_WARNING;
	lll_index = LOGLEVEL_WARNING;
	autosave_enabled = true;
	autosave_path = QDir(".");
}

LogSettings::~LogSettings()
{
}
