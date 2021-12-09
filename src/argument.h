#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

//#include "app.h"

enum class State: int8_t
{
    NONE,
    CARIBRATION,
    READY
};

struct AppArgument
{
    State state;
    State prev_state;
    
    int   screenMode     = 0;
    float bankAngle      = 0;
    bool  isSensorReady  = false;
};



#endif  // __ARGUMENT_H__