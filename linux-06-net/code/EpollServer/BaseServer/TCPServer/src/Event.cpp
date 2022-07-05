
#include "Event.h"
#include "EventHandleInterface.h"

namespace CTCPSERVER {
    //Do this event in the context of threadpool
    void Event::doit()
    {
        if(mpEventHandler)
        {
            static_cast<IEventHandler*>(mpEventHandler)->HandleEvent(*this,mpReactor);
        }
    }
}
