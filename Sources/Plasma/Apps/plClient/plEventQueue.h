//
//  plEventQueue.hpp
//  plClient
//
//  Created by Colin Cornaby on 11/2/25.
//

#ifndef plEventQueue_hpp
#define plEventQueue_hpp

#include <stdio.h>
#include <queue>

class plClient;
class plClientLoader;
class plInputManager;

struct plEventQueueContext
{
    plClientLoader *client;
    plInputManager *inputManager;
};

class plEventQueue
{
public:
    plEventQueue(plClientLoader* client);
    void AddEvent(const std::function<void(const plEventQueueContext&)> &event);
    void Drain();
private:
    std::deque<std::function<const void(const plEventQueueContext&)>> _events;
    plEventQueueContext _context;
};

#endif /* plEventQueue_hpp */
