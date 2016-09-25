#ifndef HAND_GESTURE_HPP
#define HAND_GESTURE_HPP

//C++ Includes
#include <iostream>
#include <thread>
#include <chrono>

//OS Includes
#include <unistd.h>

//PolySync Includes
#include <PolySyncNode.hpp>
#include <PolySyncMessage.hpp>
#include <PolySyncDataModel.hpp>

class HandGesture : public polysync::Node
{
    public:
        HandGesture(int argc, char * argv[]);
        ~HandGesture();
    private:
        void setConfigurationEvent(int argc, char * argv[]) override;
        void initStateEvent() override;
        void okStateEvent() override;
        void warnStateEvent() override;
        void errorStateEvent() override;
        void fatalStateEvent() override;
        void releaseStateEvent() override;
        void messageEvent(std::shared_ptr<polysync::Message> message) override;
};

#endif
