/**********************************************************
* Holo_ToolSet
* http://github.com/HasseltVR/Holo_ToolSet
* http://www.uhasselt.be/edm
*
* Distributed under LGPL v2.1 Licence
* http ://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
**********************************************************/
#pragma once

#include <functional>

namespace HPV {
    
    /* Forward declare player class for event to hold pointer */
    class HPVPlayer;
    
    /*
     * HPVEventType defines a an action that leads to triggering an HPV event
     */
    enum class HPVEventType : std::uint8_t
    {
        HPV_EVENT_PLAY = 0,
        HPV_EVENT_PAUSE,
        HPV_EVENT_STOP,
        HPV_EVENT_RESUME,
        HPV_EVENT_LOOP,
        HPV_EVENT_NUM_TYPES = 5
    };
    
    /*
     * HPVEvent defines an event that gets triggered when a certain action takes place
     * inside the engine. These actions are defined as HPVEventTypes
     */
    class HPVEvent
    {
    public:
        HPVEvent(HPVEventType _type, HPVPlayer * _player) : type(_type), player(_player) {};
        
        HPVEventType    type;
        HPVPlayer *     player;
    };
    
    /*
     * Callback for an HPVevent. Listeners can provide such a callback funtion to get
     * notified when HPV events occur
     */
    typedef std::function<void(const HPVEvent&)> HPVEventCallback;
        
} /* End HPV namespace */