
/*
 * Copyright (c) 2024 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "juce_gui_basics/juce_gui_basics.h"
#include "ipc/subsystem/mt_ipc_subsystem.h"
#include "ipc/dispatcher/mt_ipc_message_dispatcher.h"
#include "contracts/config/ipc_config_helpers.h"
#include "contracts/config/ipc_config_constants.h"
#include "ipc/utils/mt_ipc_message_types.h"
#include "ipc/utils/mt_ipc_enums.h"
#include "ipc/utils/mt_ipc_constants.h"
#include "ipc/utils/mt_ipc_helpers.h"
#include "contracts/dcl/dcl_signals.h"
#include "contracts/dcl/dcl_contracts_helper.h"
#include "contracts/dcl/dcl_ipc_interface.h"
#include "contracts/config/connection_id_store.h"
#include "PktMgr/PktMgr.hpp"
#include "contracts/utils/app/ipc_application.h"


#if JUCE_WINDOWS
    #include <Windows.h>
#endif


bool stop = false;

void dispatchMessages( ipc::MTIPCMessageDispatcher* pDisptacher )
{
    while(! pDisptacher->canDispatch( 200 ) && !stop )
        ;

    CPktMgr::HWMsg_t msg;
    msg.pktType = CPktMgr::EN_PKT_TYPE_BUTTON;
    msg.pktDir = CPktMgr::EN_PKT_REQUEST;
    msg.pktCmd  = contracts::DeviceControlContracts::SysCmd::EN_SYS_RECONNECT;

    contracts::DclSignals::createAndSendCommandToDcl(msg);

    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::reconnectMessage());
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::shutdownMessage());
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::SendSwUpdateCmdMessage(1));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setPlayStateMessage( contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_E));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::clearAllPadsMessage());
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setLEDONMessage( 100, 200));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setLEDOFFMessage( 100, 200));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::sendSliderValueInPercentMessage( 23));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setPadDataMessage(100, 12));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setPadBlinkingMessage( 100, 12, 23));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setPadSettingsMessage(std::vector<int> ({10, 20, 30})));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setTempoMessage(100) );
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setBankMessage( 4));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setMediaControlMessage( 21));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setAllPadDataColourInfoMessage( 1, 2));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setAllPadDataColourInfoMessage( 1, 3));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::
                                                 setPadColourAndStateInfoMessage( std::vector<contracts::PadData>( { { 1, contracts::DeviceControlContracts::PadButtonState::StateOn, 2 }, { 3, contracts::DeviceControlContracts::PadButtonState::StateOn, 4 } } ) ) );
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::enableAfterTouchMessage( true ));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setPadColorModeMessage( 100));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setPadColourByVelocityMessage( 100, 21));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::
        setPadColourByVelocityMessage( 4, std::vector<int>{1,2,3},  std::vector<int>{50, 30, 40}));
    contracts::DclSignals::sendCommandToDcl(contracts::DclContractsHelper::setAllPadColourByVelocityMessage( 100, 23));
    

     juce::MessageManager::callAsync( juce::JUCEApplicationBase::getInstance()->quit );
}


juce::var getStandAloneConfig()
{
    juce::Array<juce::var> clients{
        contracts::prepareClientConfig( ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET, 200,
                                        contracts::DCL_SERVER_PORT, contracts::BIND_ADRESS_OR_URL, ipc::IPC_DEFAULT_CONNECTION_TIMEOUT,
                                        "DCL TEST TO DCL" ),
    };

    juce::DynamicObject* pConnectionConfig = new juce::DynamicObject();
    pConnectionConfig->setProperty( ipc::IPC_KEY_UUID, "9923c2d5a5ed460680dafca091e95b51" );
    pConnectionConfig->setProperty( ipc::IPC_KEY_DELAY_STARTING_CLIENT, false );
    pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENT_START_DELAY, contracts::CLIENT_START_DELAY );
    pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENTS, clients );
    juce::DynamicObject* pObj = new juce::DynamicObject();
    pObj->setProperty( ipc::IPC_KEY_CONNECTION_CONFIG, pConnectionConfig );
    pObj->setProperty( contracts::CONFIG_KEY_CONNECTION_IDS, contracts::prepareConnectionIdConfig(-1, -1, -1, 200, -1, -1, -1 ) );
    return pObj;
}


//==============================================================================
class DeviceControlTest : public contracts::IPCApplication
{
public:
    //==============================================================================
    DeviceControlTest() : IPCApplication("KickDeviceControlTest", "KickDeviceControlTest", "KickDeviceControlTest", true) {}

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    //==============================================================================
    void initialise( const juce::String& commandLine ) override
    {
        IPCApplication::initialise(commandLine);

        dclInterface.reset( new contracts::DclIPCInterface( subsystem->dispatcher(), contracts::ConnectionIdStore::DCL_CONNECTION_ID ) );

        th = new std::thread( dispatchMessages, subsystem->dispatcher().get() );
    }

    void addMessageHandlers( std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>> & handlers){}

    juce::var getConfig( const juce::String& commandLine )
    {
        return getStandAloneConfig();
    }

    void shutdown() override
    {
        stop = true;
        th->join();
        delete th;
        th = nullptr;
        dclInterface.reset();
        subsystem.reset();
        if( logger )
        {
            juce::Logger::setCurrentLogger( nullptr );
            logger.reset();
        }
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // delete the communication modules
    }

    void anotherInstanceStarted( const juce::String& /*commandLine*/ ) override
    {
        // When another instance of the app is launched while this one is running, this method is invoked,
        // and the commandLine parameter tells you what the other instance's command-line arguments were.
    }
private:
    std::unique_ptr<ipc::MTIPCSubsystem>                   subsystem{ nullptr };
    std::unique_ptr<contracts::DclIPCInterface>            dclInterface{ nullptr };
    std::thread*                                           th{ nullptr };
    std::unique_ptr<ipc::MTIPCLogger>                      logger{ nullptr };

   };

//==============================================================================
// This macro generates the main() routine that launches the app.
   START_JUCE_APPLICATION( DeviceControlTest )
