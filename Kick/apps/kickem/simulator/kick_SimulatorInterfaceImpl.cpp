#include "kick_SimulatorInterfaceImpl.h"

namespace sim
{
SimulatorIntrefaceImpl* SimulatorIntrefaceImpl::inst = nullptr;
void SimulatorIntrefaceImpl::sendMessageToHW(int id, const std::vector<juce::int8>& data)
{
    hwMessageReceived.emit(id, data);
}

SimulatorIntrefaceImpl::SimulatorIntrefaceImpl()
{
    inst = this;
}
REGISER_FACTORY(SimulatorInterface, SimulatorIntrefaceImpl, 0);
}