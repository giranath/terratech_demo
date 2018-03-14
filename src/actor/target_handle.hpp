#ifndef DEF_TARGET_HANDLE_HPP
#define DEF_TARGET_HANDLE_HPP



class unit_manager;
class base_unit;
class target_handle
{
    unit_manager* manager;
    uint32_t id; 

public:
    
    target_handle() = delete;
    target_handle(unit_manager* manager);

    target_handle(unit_manager* manager, base_unit* unit);

    void set_unit_manager(unit_manager* _manager);

    base_unit* get();

    void set(base_unit* unit);
};

#endif