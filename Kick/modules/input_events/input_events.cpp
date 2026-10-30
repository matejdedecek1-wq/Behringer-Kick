
#include <moodycamel/moodycamel.h>
#include <juce_serialport/juce_serialport.h>

#include "input_events.h"
#include "contracts/dcl/dcl_contracts.h"
#include "contracts/dcl/dcl_contracts_helper.h"
#include "kick_SimulatorInterface.h"

#include "ComConfig.hpp"

// The serial link to the control-surface microcontroller: compiled wherever juce_serialport has a
// backend (the POSIX build and the Win32 build of this tree). On the unit it is /dev/ttymxc0;
// KICK_SERIAL_DEVICE names another device (a USB adapter, or the pseudo-terminal of a simulated
// panel).
#if JUCE_WINDOWS || JUCE_LINUX || __linux__
    #define KICK_SERIAL_LINK 1
#else
    #define KICK_SERIAL_LINK 0
#endif

namespace Controls
{
    std::atomic<bool> shiftBtnState{ false };
    std::atomic<bool> halfLevelState{ false };

    ///
    /// Static members...
    static std::array<uint8_t, BUFFER_RX_SIZE> ms_arrui8RxBufferUsb;
    static std::array<uint8_t, BUFFER_RX_SIZE> ms_arrui8TxBufferUsb;
    static SimulatorInterface*                 simInterface{ nullptr };
    static CPktMgr                             ms_objPktMgrUsb{ CPktMgr::EN_COM_CHANNEL_USB, ms_arrui8RxBufferUsb.data(), BUFFER_RX_SIZE, ms_arrui8TxBufferUsb.data(), BUFFER_RX_SIZE, 60 };

    sigslot::signal<CPktMgr::HWMsg_t&> Controls::InputEvents::dispatchHardwareEvent = {};

    ///
    /// IMPL...
    class HwSerialData : private juce::ChangeListener
        , private juce::Thread
        , private juce::AsyncUpdater
    {
    public:
        HwSerialData()
            : Thread( "Queue" ) { startThread(); }

        ~HwSerialData()
        {
#ifndef __APPLE__
            close();
#endif
        }

        void connect()
        {
            // The send thread: close() stops it, so a reconnect() must start it again.
            if( !isThreadRunning() )
                startThread();
#if KICK_SERIAL_LINK
            // KICK_SERIAL_DEVICE=<absolute path> replaces the unit's port; unset, it is
            // /dev/ttymxc0 as always. Same settings and handshake either way.
            const juce::String overridePath = juce::SystemStats::getEnvironmentVariable( "KICK_SERIAL_DEVICE", {} ).trim();
            const juce::String serialDev    = overridePath.isNotEmpty() ? overridePath : juce::String( "/dev/ttymxc0" );
            const juce::String origin       = overridePath.isNotEmpty() ? "KICK_SERIAL_DEVICE" : "default";

            if( juce::File::isAbsolutePath( serialDev ) && juce::File( serialDev ).exists() )
            {
                juce::StringPairArray portlist = SerialPort::getSerialPortPaths();

                // The unit's own port is listed in /dev; a pseudo-terminal named by
                // KICK_SERIAL_DEVICE (/dev/pts/N, /dev/ttysNNN) is not, and needs no listing.
                if( !portlist.size() && overridePath.isEmpty() )
                    return;

                auto serialDebug = []( juce::String str1, juce::String str2 ) {
                    juce::Logger::writeToLog( str1 + ": " + str2 );
                };
                SerialPortConfig config( 230400, 8, SerialPortConfig::SERIALPORT_PARITY_NONE, SerialPortConfig::STOPBITS_1, SerialPortConfig::FLOWCONTROL_NONE );

                juce::String devPath = serialDev;
                if( sPort )
                {
                    input.reset( nullptr );
                    output.reset( nullptr );
                    sPort.reset( nullptr );
                }
                {
                    CPktMgr::HWMsg_t hwMsg;
                    hwMsg.pktType = CPktMgr::EN_PKT_TYPE_SYSTEM;
                    hwMsg.pktCmd  = contracts::DeviceControlContracts::SysCmd::EN_SYS_COMM_INIT;
                    hwMsg.pktDir  = CPktMgr::EN_PKT_REQUEST;

                    sPort.reset( new SerialPort( devPath, config, serialDebug ) );
                    juce::Logger::writeToLog( "InputEvents: control surface on " + devPath + " (" + origin + "): " + ( sPort->exists() ? "open" : "could not open" ) );
                    input.reset( new SerialPortInputStream( sPort.get() ) );
                    input->addChangeListener( this );
                    input->setNotify( SerialPortInputStream::NOTIFY_ALWAYS );

                    hwMsg.data.push_back( (int)contracts::DeviceControlContracts::SysCommCmd::EN_SYS_COMM_CONNECTED );
                    output.reset( new SerialPortOutputStream( sPort.get() ) );
                    Controls::InputEvents::sendMessageToHardware( hwMsg );
                }
            }
            else
            {
                DBG( "Serial Not Found" );
                if( overridePath.isNotEmpty() && !missingLogged )
                    juce::Logger::writeToLog( "InputEvents: KICK_SERIAL_DEVICE=" + overridePath + " does not exist (an absolute path is needed)" );
                missingLogged = true;
            }
#endif
        }

        void close()
        {
            if( sPort ) sPort->close();
            stopThread( 1000 );
            waitForThreadToExit( 1000 );
        }

        void reconnect()
        {
#ifndef __APPLE__
            close();
            connect();
#endif
        }

        void sendMessageToControlSurface( CPktMgr::HWMsg_t& f_HwMsg )
        { // Enqueue in the queue...
            txQueue.enqueue( f_HwMsg );
            //semaphoreQueue.signal();
            triggerAsyncUpdate();
        }

        void messageReceivedFromControlSurface( CPktMgr::HWMsg_t& f_rxMsg )
        {
            juce::String log;
            log += " Data[0] : " + juce::String( f_rxMsg.data[0] ) + " Cmd : " + juce::String( f_rxMsg.pktCmd ) + +" type : " + juce::String( f_rxMsg.pktType );
            juce::Logger::writeToLog( log );

            Controls::InputEvents::dispatchHardwareEvent( f_rxMsg );
        }

    private:
        void handleAsyncUpdate() override { txEvent.signal(); }
        void sendSerialOutData( const std::vector<char>& data )
        {
#if KICK_SERIAL_LINK
            if( !output )
                return;

            juce::MemoryBlock outData;
            uint8_t           f_txSize = data.size();
            //if(f_txSize > 0)
            //{
            //p + (4 int size) + (hw id) + data + \n
            outData.ensureSize( f_txSize );
            //DBG("======TX==Message======");
            for( unsigned int i = 0; i < f_txSize; i++ )
            {
                outData[i] = data[i];
                //DBG("TxData: " << (int)outData[i]);
            }

            output->write( outData.getData(), outData.getSize() );
            output->flush();
            //}
#endif
        }

        void changeListenerCallback( juce::ChangeBroadcaster* source ) override
        {
            int rxLength = (int)input->getTotalLength();
            // DBG("Receive Length: " << rxLength);
            if( !input->getTotalLength() || input->getTotalLength() < 6 )
                return;

            juce::MemoryBlock serialData( (size_t)input->getTotalLength(), true );
            size_t            len = (size_t)input->getTotalLength();

            input->read( serialData.getData(), (int)input->getTotalLength() );

            if( len < 4 )
            {
                DBG( "Size is less then 3 bytes" );
                return;
            }

            char* byteArray = serialData.begin();

            CPktMgr& refobjFrameHdl = ms_objPktMgrUsb;

            while( rxLength )
            {
                int pktIndex     = 0;
                int pktOffset    = 0;
                int packetLength = byteArray[5 + pktIndex] + 7;
                // DBG("Packet Length: " << packetLength);

                if( ( rxLength >= packetLength ) && ( byteArray[0] == 'M' ) && ( byteArray[1] == 'T' ) )
                {
                    while( packetLength )
                    {
                        if( packetLength > 1 )
                        {
                            refobjFrameHdl.PushByteToRxFifo( byteArray[0] );
                        }
                        byteArray++;
                        packetLength--;
                        rxLength--;
                    }
                }
                else
                {
                    rxLength = 0;
                }

                if( refobjFrameHdl.IsFrameRcvComplete() == TRUE )
                {
                    refobjFrameHdl.SetFrameRcvComplete( FALSE );
                    CPktMgr::HWMsg_t rxMsg;
                    std::vector<int> tempData;
                    tempData.push_back( 1 );
                    rxMsg.pktType = refobjFrameHdl.GetRxPktType();
                    rxMsg.pktCmd  = refobjFrameHdl.GetRxCommand();
                    rxMsg.pktDir  = refobjFrameHdl.GetPktDir();

                    int size = (int)refobjFrameHdl.GetRxFrameSize();

                    /*DBG( "===========RX Message==========" );
                    DBG( "Rx Type: " << rxMsg.pktType << "Rx pktId: " << rxMsg.pktCmd << "Rx size: " << size );
                   */
                    //juce::Logger::writeToLog( "Rx Type: " + juce::String(rxMsg.pktType) + "Rx pktId: " + juce::String(rxMsg.pktCmd) + "Rx size: " + juce::String(size ));

                    int index = 0;
                    while( size - 1 )
                    {
                        rxMsg.data.push_back( (int)refobjFrameHdl.PopUint8FromRxFifo() );
                        index++;
                        size--;
                    }
                    refobjFrameHdl.ClearReceiver();
                    // DBG("Pad Num: "<<  rxMsg.data[0]);

                    if( rxMsg.pktDir == CPktMgr::enumPacketDir::EN_PKT_REQUEST )
                    {
                        CPktMgr::HWMsg_t ackTxMsg;
                        //ackTxMsg.pktType = rxMsg.pktType;
                        //ackTxMsg.pktDir  = CPktMgr::enumPacketDir::EN_PKT_RESPONSE;
                        //ackTxMsg.pktId     = rxMsg.pktId;
                        //ackTxMsg.data      = rxMsg.data;

                        //DBG("Ack Msg");
                        //sendMessageToHardware( ackTxMsg );

                        if( contracts::DclContractsHelper::isButtonMessage( rxMsg ) )
                        {
                            DBG( "Button state = " << contracts::DclContractsHelper::getButtonState( rxMsg ) );
                            if( ( contracts::DeviceControlContracts::EN_SHIFT == contracts::DclContractsHelper::getButtonCode( rxMsg ) ) )
                                Controls::shiftBtnState.store( contracts::DclContractsHelper::getButtonState( rxMsg ) );
                        }
                        // juce::String log;
                        // log += "Cmd:" + juce::String( rxMsg.pktCmd ) + " " + "type:" + juce::String( rxMsg.pktType );
                        // juce::Logger::writeToLog( log );
                        messageReceivedFromControlSurface( rxMsg );
                    }
                    else
                    {
                        //If the data is Ack (response) for the App message, ignore
                        // Do Nothing
                    }
                }
                else
                {
#if defined( DEBUG_PRINTS ) && ( DEBUG_PRINTS > 0 )
                    DBG( "Data Frame Error" );
#endif
                    refobjFrameHdl.ClearReceiver();
                }
            }
        }

        void run() override
        {
            static CPktMgr::HWMsg_t  item;
            static std::vector<char> txData;
            while( !threadShouldExit() )
            {
                txEvent.wait( 1000 );
                //semaphoreQueue.wait( 1 );

                while( txQueue.try_dequeue( item ) && !threadShouldExit() )
                {
#if KICK_SERIAL_LINK
                    txData.clear();
                    ms_objPktMgrUsb.BuildAndTransmitPacket( item, txData );
                    sendSerialOutData( txData );

                    item.data.clear();
#endif
                }
            } //DBG("Out of While");
        }

    private:
        juce::WaitableEvent txEvent;

        std::unique_ptr<SerialPort>             sPort;
        std::unique_ptr<SerialPortInputStream>  input;
        std::unique_ptr<SerialPortOutputStream> output;

        moodycamel::ConcurrentQueue<CPktMgr::HWMsg_t> txQueue;

        bool missingLogged{ false };
    };
    static std::unique_ptr<HwSerialData> _FPPort;

    /// <summary>
    ///
    /// Class Controls::InputEvents
    ///
    /// </summary>

    void Controls::InputEvents::initialize()
    {
#ifdef SIM_BUILD
        //creating multiple instance of SimulatorInterface using factoty is somehow clearing the connected signals need to find the reason
        simInterface = Factory<SimulatorInterface>::makeObject( 0 );
        simInterface->hwInputReceived.connect( []( int id, const std::vector<char>& data ) {
            Controls::InputEvents::dispatchHardwareEvent( id, data );
        } );
#else
        if( !_FPPort )
        {
            _FPPort.reset( new HwSerialData );
            ms_objPktMgrUsb.Init();
        }
        _FPPort->connect();
        // Send Rebooted command to GD
#endif
    }

    void Controls::InputEvents::reconnect()
    {
#ifndef __APPLE__
        if( _FPPort ) _FPPort->reconnect();
#endif
    }

    void Controls::InputEvents::shutdown()
    {
        _FPPort.reset();
    }

    void Controls::InputEvents::messageReceived( CPktMgr::HWMsg_t& f_rxMsg )
    {
        if( _FPPort ) _FPPort->messageReceivedFromControlSurface( f_rxMsg );
    }

    void Controls::InputEvents::sendMessageToHardware( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( f_HwMsg.pktType == CPktMgr::enumPacketType::EN_PKT_TYPE_SYSTEM )
        {
            if( f_HwMsg.pktCmd == contracts::DeviceControlContracts::EN_SYS_RECONNECT )
            {
                reconnect();
                return;
            }
            if( f_HwMsg.pktCmd == contracts::DeviceControlContracts::EN_SYS_SHUTDOWN )
            {
                shutdown();
                return;
            }
        }

#ifdef SIM_BUILD
        if( simInterface ) simInterface->sendMessageToHW( f_HwMsg );
#endif
        if( _FPPort ) _FPPort->sendMessageToControlSurface( f_HwMsg );
        else
        {
            juce::Logger::outputDebugString( "Null Serial Port " );
        }
    }
}
