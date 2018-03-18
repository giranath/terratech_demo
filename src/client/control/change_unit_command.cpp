#include "change_unit_command.hpp"

namespace input{
	change_unit_command::change_unit_command(int& next_to_spawn, int unit_to_spawn)
		: next_unit_to_spawn(next_to_spawn)
		, unit_to_spawn(unit_to_spawn) {

	}

	void change_unit_command::execute()
	{
		next_unit_to_spawn = unit_to_spawn;
	}
}