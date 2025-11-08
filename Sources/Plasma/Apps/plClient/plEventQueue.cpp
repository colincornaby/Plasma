//
//  plEventQueue.cpp
//  plClient
//
//  Created by Colin Cornaby on 11/2/25.
//

#include "plClient.h"
#include "plClientLoader.h"
#include "plEventQueue.h"

plEventQueue::plEventQueue(plClientLoader* client)
{
    _context.client = client;
    _context.inputManager = (*client)->GetInputManager();
}

void plEventQueue::AddEvent(const std::function<void(const plEventQueueContext&)> &event) {
    _events.push_back(std::move(event));
}

void plEventQueue::Drain() { 
    while(!_events.empty())
    {
        _events.front()(_context);
        _events.pop_front();
    }
}
