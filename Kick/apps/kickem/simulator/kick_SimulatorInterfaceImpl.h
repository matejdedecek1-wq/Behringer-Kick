#pragma once
#include <kick_ui/kick_ui.h>

namespace sim
{
class SimulatorIntrefaceImpl : public SimulatorInterface
{
public:
    SimulatorIntrefaceImpl();
    void* operator new(size_t s)
    {
        if (inst)
            return inst;
        else
            return malloc(s);
    }
    void sendMessageToHW(int code, const std::vector<juce::int8>&) override;

SIGNALS:
    inline static sig::Signal<int, const std::vector<juce::int8>&> hwMessageReceived = {};

private:
    static SimulatorIntrefaceImpl* inst;
};
}
