#ifndef MMAP_DEMO_CHANGE_UNIT_COMMAND_HPP
#define MMAP_DEMO_CHANGE_UNIT_COMMAND_HPP

#include "command.hpp"

namespace input {
	class change_unit_command : public command {
		int& next_unit_to_spawn;
		int unit_to_spawn;
	public:
		change_unit_command(int& next_to_spawn, int unit_to_spawn);
		void execute() override;

	};

}
#endif //MMAP_DEMO_CHANGE_UNIT_COMMAND_HPP
