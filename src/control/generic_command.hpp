#ifndef MMAP_DEMO_GENERIC_COMMAND_HPP
#define MMAP_DEMO_GENERIC_COMMAND_HPP

#include "command.hpp"
#include <memory>

namespace input {
	template <typename fn>
	class generic_command : public command {
		fn function;
	public:
		generic_command(fn func)
			: function(func) {
		};

		void execute() override {
			function(); 
		}
	};

	template<typename FN>
	std::unique_ptr<input::generic_command<FN>> make_generic_command(FN&& fn) {
		return std::make_unique<input::generic_command<FN>>(std::forward<FN>(fn));
	}

}
#endif //MMAP_DEMO_GENERIC_COMMAND_HPP
