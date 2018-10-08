#pragma once

#include "sqlhandlertraits.hxx"

namespace TUESL::SQLite::Handler
{
	using Database = TUESL::Utility::Handler::UniqueHandler<Traits::SQLiteDatabaseHandlerTraits>;
	using PrepareStatement = TUESL::Utility::Handler::UniqueHandler<Traits::SQLitePrepareStatementHandlerTraits>;
}
