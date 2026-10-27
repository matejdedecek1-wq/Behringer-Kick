/*******************************************************************************
 A replacement for the juce_serialport module that was removed from the 0700-AAM
 release because it carried no licence (see Kick/README.md, "Removed from this
 release"). Written from the calls Kick makes, not from the original.

 The stream/port API is the one Kick uses; the POSIX implementation is the
 community macOS port's (github.com/Eight4aWish/0700-AAM, branch macos-build).
 The Win32 implementation (used by this Windows build) is new here and covers the
 same surface. No device is attached, so on Windows the port list is whatever
 QueryDosDevice reports and opening a real COM port is untested against hardware.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_serialport
  vendor:             Eight4aWish
  version:            0.2.0
  name:               Serial port streams (POSIX + Win32)
  description:        A serial port with JUCE input and output streams, for the KICK panel link
  website:            https://github.com/Eight4aWish
  license:            AGPLv3

  dependencies:       juce_core, juce_events

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#define JUCE_SERIALPORT_H_INCLUDED

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <functional>

class SerialPortConfig
{
public:
    enum SerialPortParity { SERIALPORT_PARITY_NONE, SERIALPORT_PARITY_ODD, SERIALPORT_PARITY_EVEN };
    enum SerialPortStopBits { STOPBITS_1, STOPBITS_2 };
    enum SerialPortFlowControl { FLOWCONTROL_NONE, FLOWCONTROL_HARDWARE, FLOWCONTROL_XONXOFF };

    SerialPortConfig() = default;
    SerialPortConfig( int bps_, int databits_, SerialPortParity parity_, SerialPortStopBits stopbits_,
                      SerialPortFlowControl flowcontrol_ )
        : bps( bps_ ), databits( databits_ ), parity( parity_ ), stopbits( stopbits_ ), flowcontrol( flowcontrol_ )
    {
    }

    int                   bps{ 9600 };
    int                   databits{ 8 };
    SerialPortParity      parity{ SERIALPORT_PARITY_NONE };
    SerialPortStopBits    stopbits{ STOPBITS_1 };
    SerialPortFlowControl flowcontrol{ FLOWCONTROL_NONE };
};

class SerialPort
{
public:
    using DebugFunction = std::function<void( juce::String, juce::String )>;

    SerialPort( const juce::String& path, const SerialPortConfig& config, DebugFunction debug = nullptr );
    ~SerialPort();

    bool open( const juce::String& path );
    void close();
    bool exists() const;
    bool setConfig( const SerialPortConfig& config );

    // Display name -> device path, as the original returned them.
    static juce::StringPairArray getSerialPortPaths();

#if JUCE_WINDOWS
    // A Win32 HANDLE, kept as void* so this header does not pull in <windows.h>.
    void* getNativeHandle() const { return handle; }
#else
    int getFileDescriptor() const { return fd; }
#endif

private:
    void debug( const juce::String& a, const juce::String& b ) const
    {
        if( debugFunction ) debugFunction( a, b );
    }

    juce::String  portPath;
    DebugFunction debugFunction;

#if JUCE_WINDOWS
    void* handle{ nullptr };
#else
    int fd{ -1 };
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SerialPort )
};

// Reads on its own thread and broadcasts a change whenever bytes arrive.
class SerialPortInputStream : public juce::InputStream, public juce::ChangeBroadcaster, private juce::Thread
{
public:
    enum NotifyMode { NOTIFY_OFF, NOTIFY_ON_CHAR, NOTIFY_ALWAYS };

    explicit SerialPortInputStream( SerialPort* port );
    ~SerialPortInputStream() override;

    void setNotify( NotifyMode mode, char c = 0 );

    // Bytes waiting in the buffer, not the size of a stream.
    juce::int64 getTotalLength() override;
    bool        isExhausted() override;
    int         read( void* destBuffer, int maxBytesToRead ) override;
    juce::int64 getPosition() override { return 0; }
    bool        setPosition( juce::int64 ) override { return false; }

private:
    void run() override;

    SerialPort*            port;
    juce::CriticalSection  bufferLock;
    juce::MemoryBlock      buffer;
    size_t                 bufferedBytes{ 0 };
    std::atomic<NotifyMode> notify{ NOTIFY_OFF };
    char                   notifyChar{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SerialPortInputStream )
};

class SerialPortOutputStream : public juce::OutputStream
{
public:
    explicit SerialPortOutputStream( SerialPort* port_ ) : port( port_ ) {}

    void        flush() override;
    bool        write( const void* data, size_t numBytes ) override;
    juce::int64 getPosition() override { return written; }
    bool        setPosition( juce::int64 ) override { return false; }

private:
    SerialPort* port;
    juce::int64 written{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SerialPortOutputStream )
};
