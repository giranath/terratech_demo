#ifndef DEF_COMMAND_HPP
#define DEF_COMMAND_HPP

class command
{
public:
    virtual void execute() = 0;
    virtual bool is_repeatable() = 0;
    virtual ~command() {}
};

#endif