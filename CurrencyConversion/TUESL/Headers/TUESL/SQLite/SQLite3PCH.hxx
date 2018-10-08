#pragma once

#if __has_include(<winsqlite\winsqlite3.h>)
#	include <winsqlite\winsqlite3.h>
#	ifdef _MSC_VER
#		pragma comment(lib, "winsqlite3.lib")
#	endif
#else
#	include <SQLite3\sqlite3.h> //Included if Windows SQLite Header absent
#endif

#include <string_view>
