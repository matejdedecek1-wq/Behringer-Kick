#pragma once

class SimulatorInterface
{
public:
    virtual ~SimulatorInterface() {}
    virtual void sendMessageToHW( int code, const std::vector<juce::int8>& ) = 0;

    SIGNALS : sigslot::signal<int, const std::vector<char>&> hwInputReceived;
};
