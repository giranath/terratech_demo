#ifndef DEF_COMMAND_HPP
#define DEF_COMMAND_HPP

namespace input {

class command {
public:
    virtual void execute() = 0;

    virtual ~command() {}
};

}

#endif