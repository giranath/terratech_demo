#include "unit_manager.hpp"
#include "target_handle.hpp"
#include "base_unit.hpp"

target_handle::target_handle(unit_manager* manager) :
    id{},
    manager{manager}
{}

target_handle::target_handle(unit_manager* manager, base_unit* unit) :
    
    manager{ manager },
    id{unit->get_id()}

{}

void target_handle::set_unit_manager(unit_manager* _manager)
{
    manager = _manager;
}

base_unit* target_handle::get()
{
    return manager->get(id);
}

void target_handle::set(base_unit* unit)
{
    id = unit->get_id();
}