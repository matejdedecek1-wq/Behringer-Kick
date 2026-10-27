#include "juce_serialport.h"

#if JUCE_WINDOWS
    #include <windows.h>
    #define SERIALPORT_WINDOWS 1
    #define SERIALPORT_POSIX 0
#elif JUCE_LINUX || JUCE_MAC || JUCE_BSD
    #include <fcntl.h>
    #include <poll.h>
    #include <termios.h>
    #include <unistd.h>
    #define SERIALPORT_WINDOWS 0
    #define SERIALPORT_POSIX 1
#else
    #define SERIALPORT_WINDOWS 0
    #define SERIALPORT_POSIX 0
#endif

#if SERIALPORT_POSIX
static speed_t toSpeed( int bps )
{
    switch( bps )
    {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default: return B9600;
    }
}
#endif

SerialPort::SerialPort( const juce::String& path, const SerialPortConfig& config, DebugFunction debug_ )
    : debugFunction( std::move( debug_ ) )
{
    if( open( path ) )
        setConfig( config );
}

SerialPort::~SerialPort() { close(); }

bool SerialPort::open( const juce::String& path )
{
    close();
    portPath = path;
#if SERIALPORT_WINDOWS
    HANDLE h = ::CreateFileW( path.toWideCharPointer(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                              OPEN_EXISTING, 0, nullptr );
    if( h == INVALID_HANDLE_VALUE )
    {
        debug( "SerialPort::open failed", path );
        return false;
    }
    handle = h;
    return true;
#elif SERIALPORT_POSIX
    fd = ::open( path.toRawUTF8(), O_RDWR | O_NOCTTY | O_NONBLOCK );
    if( fd < 0 )
    {
        debug( "SerialPort::open failed", path );
        return false;
    }
    return true;
#else
    debug( "SerialPort: no implementation on this platform", path );
    return false;
#endif
}

void SerialPort::close()
{
#if SERIALPORT_WINDOWS
    if( handle != nullptr )
        ::CloseHandle( (HANDLE)handle );
    handle = nullptr;
#elif SERIALPORT_POSIX
    if( fd >= 0 )
        ::close( fd );
    fd = -1;
#endif
}

bool SerialPort::exists() const
{
#if SERIALPORT_WINDOWS
    return handle != nullptr;
#else
    return fd >= 0;
#endif
}

bool SerialPort::setConfig( const SerialPortConfig& config )
{
#if SERIALPORT_WINDOWS
    if( handle == nullptr ) return false;

    HANDLE h = (HANDLE)handle;
    DCB    dcb{};
    dcb.DCBlength = sizeof( DCB );
    if( !::GetCommState( h, &dcb ) ) return false;

    dcb.BaudRate = (DWORD)config.bps;
    dcb.ByteSize = (BYTE)config.databits;
    dcb.fBinary  = TRUE;
    dcb.fParity  = ( config.parity == SerialPortConfig::SERIALPORT_PARITY_NONE ) ? FALSE : TRUE;
    dcb.Parity   = config.parity == SerialPortConfig::SERIALPORT_PARITY_EVEN ? EVENPARITY
                 : config.parity == SerialPortConfig::SERIALPORT_PARITY_ODD  ? ODDPARITY
                                                                            : NOPARITY;
    dcb.StopBits = config.stopbits == SerialPortConfig::STOPBITS_2 ? TWOSTOPBITS : ONESTOPBIT;

    dcb.fOutxCtsFlow = config.flowcontrol == SerialPortConfig::FLOWCONTROL_HARDWARE ? TRUE : FALSE;
    dcb.fRtsControl  = config.flowcontrol == SerialPortConfig::FLOWCONTROL_HARDWARE ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_ENABLE;
    dcb.fOutX        = config.flowcontrol == SerialPortConfig::FLOWCONTROL_XONXOFF ? TRUE : FALSE;
    dcb.fInX         = config.flowcontrol == SerialPortConfig::FLOWCONTROL_XONXOFF ? TRUE : FALSE;

    if( !::SetCommState( h, &dcb ) ) return false;

    COMMTIMEOUTS timeouts{};
    timeouts.ReadIntervalTimeout         = 10;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 100;
    ::SetCommTimeouts( h, &timeouts );
    return true;
#elif SERIALPORT_POSIX
    if( fd < 0 ) return false;

    termios tio{};
    if( tcgetattr( fd, &tio ) != 0 ) return false;
    cfmakeraw( &tio );
    cfsetispeed( &tio, toSpeed( config.bps ) );
    cfsetospeed( &tio, toSpeed( config.bps ) );

    tio.c_cflag &= ~(tcflag_t)CSIZE;
    tio.c_cflag |= config.databits == 7 ? CS7 : CS8;
    tio.c_cflag |= CREAD | CLOCAL;

    tio.c_cflag &= ~(tcflag_t)( PARENB | PARODD );
    if( config.parity == SerialPortConfig::SERIALPORT_PARITY_EVEN ) tio.c_cflag |= PARENB;
    if( config.parity == SerialPortConfig::SERIALPORT_PARITY_ODD ) tio.c_cflag |= PARENB | PARODD;

    if( config.stopbits == SerialPortConfig::STOPBITS_2 ) tio.c_cflag |= CSTOPB;
    else tio.c_cflag &= ~(tcflag_t)CSTOPB;

    tio.c_cflag &= ~(tcflag_t)CRTSCTS;
    tio.c_iflag &= ~(tcflag_t)( IXON | IXOFF );
    if( config.flowcontrol == SerialPortConfig::FLOWCONTROL_HARDWARE ) tio.c_cflag |= CRTSCTS;
    if( config.flowcontrol == SerialPortConfig::FLOWCONTROL_XONXOFF ) tio.c_iflag |= IXON | IXOFF;

    tio.c_cc[VMIN]  = 0;
    tio.c_cc[VTIME] = 0;
    return tcsetattr( fd, TCSANOW, &tio ) == 0;
#else
    juce::ignoreUnused( config );
    return false;
#endif
}

juce::StringPairArray SerialPort::getSerialPortPaths()
{
    juce::StringPairArray ports;
#if SERIALPORT_WINDOWS
    // QueryDosDevice lists every DOS device name without opening anything; the panel link
    // lives on a COM port, so keep "COM" + digits.
    std::vector<wchar_t> names( 65536, 0 );
    const DWORD          chars = ::QueryDosDeviceW( nullptr, names.data(), (DWORD)names.size() );

    const wchar_t* p   = names.data();
    const wchar_t* end = names.data() + chars;
    while( p < end && *p != 0 )
    {
        const juce::String device( p );
        if( device.startsWith( "COM" ) && device.substring( 3 ).isNotEmpty()
            && device.substring( 3 ).containsOnly( "0123456789" ) )
            ports.set( device, "\\\\.\\" + device );
        p += device.length() + 1;
    }
#elif SERIALPORT_POSIX
    juce::File dev( "/dev" );
    #if JUCE_MAC
    const juce::String pattern = "cu.*";
    #else
    const juce::String pattern = "tty*";
    #endif
    for( const auto& f : dev.findChildFiles( juce::File::findFiles, false, pattern ) )
    {
        const auto name = f.getFileName();
        #if JUCE_LINUX || JUCE_BSD
        if( !( name.startsWith( "ttymxc" ) || name.startsWith( "ttyUSB" ) || name.startsWith( "ttyACM" ) || name.startsWith( "ttyS" ) ) )
            continue;
        #endif
        ports.set( name, f.getFullPathName() );
    }
#endif
    return ports;
}

SerialPortInputStream::SerialPortInputStream( SerialPort* port_ )
    : juce::Thread( "SerialPortInputStream" ), port( port_ )
{
    startThread();
}

SerialPortInputStream::~SerialPortInputStream() { stopThread( 1000 ); }

void SerialPortInputStream::setNotify( NotifyMode mode, char c )
{
    notifyChar = c;
    notify     = mode;
}

juce::int64 SerialPortInputStream::getTotalLength()
{
    const juce::ScopedLock sl( bufferLock );
    return (juce::int64)bufferedBytes;
}

bool SerialPortInputStream::isExhausted() { return getTotalLength() == 0; }

int SerialPortInputStream::read( void* destBuffer, int maxBytesToRead )
{
    const juce::ScopedLock sl( bufferLock );
    const auto n = (size_t)juce::jmin( (juce::int64)maxBytesToRead, (juce::int64)bufferedBytes );
    if( n == 0 ) return 0;
    std::memcpy( destBuffer, buffer.getData(), n );
    std::memmove( buffer.getData(), static_cast<char*>( buffer.getData() ) + n, bufferedBytes - n );
    bufferedBytes -= n;
    return (int)n;
}

void SerialPortInputStream::run()
{
#if SERIALPORT_WINDOWS
    char chunk[512];
    while( !threadShouldExit() )
    {
        if( port == nullptr || !port->exists() )
        {
            wait( 100 );
            continue;
        }

        HANDLE  h      = (HANDLE)port->getNativeHandle();
        COMSTAT stat{};
        DWORD   errors = 0;
        if( !::ClearCommError( h, &errors, &stat ) || stat.cbInQue == 0 )
        {
            wait( 5 );
            continue;
        }

        const DWORD want = juce::jmin( (DWORD)sizeof( chunk ), stat.cbInQue );
        DWORD       got  = 0;
        if( !::ReadFile( h, chunk, want, &got, nullptr ) || got == 0 )
        {
            wait( 5 );
            continue;
        }

        bool sawChar = false;
        {
            const juce::ScopedLock sl( bufferLock );
            if( buffer.getSize() < bufferedBytes + (size_t)got )
                buffer.setSize( ( bufferedBytes + (size_t)got ) * 2 );
            std::memcpy( static_cast<char*>( buffer.getData() ) + bufferedBytes, chunk, (size_t)got );
            bufferedBytes += (size_t)got;
            sawChar = std::memchr( chunk, notifyChar, (size_t)got ) != nullptr;
        }
        if( notify == NOTIFY_ALWAYS || ( notify == NOTIFY_ON_CHAR && sawChar ) )
            sendChangeMessage();
    }
#elif SERIALPORT_POSIX
    char chunk[512];
    while( !threadShouldExit() )
    {
        if( port == nullptr || !port->exists() )
        {
            wait( 100 );
            continue;
        }
        pollfd pfd{ port->getFileDescriptor(), POLLIN, 0 };
        if( ::poll( &pfd, 1, 100 ) <= 0 ) continue;

        const auto got = ::read( port->getFileDescriptor(), chunk, sizeof( chunk ) );
        if( got <= 0 ) continue;

        bool sawChar = false;
        {
            const juce::ScopedLock sl( bufferLock );
            if( buffer.getSize() < bufferedBytes + (size_t)got )
                buffer.setSize( ( bufferedBytes + (size_t)got ) * 2 );
            std::memcpy( static_cast<char*>( buffer.getData() ) + bufferedBytes, chunk, (size_t)got );
            bufferedBytes += (size_t)got;
            sawChar = std::memchr( chunk, notifyChar, (size_t)got ) != nullptr;
        }
        if( notify == NOTIFY_ALWAYS || ( notify == NOTIFY_ON_CHAR && sawChar ) )
            sendChangeMessage();
    }
#endif
}

void SerialPortOutputStream::flush()
{
#if SERIALPORT_WINDOWS
    if( port != nullptr && port->exists() ) ::FlushFileBuffers( (HANDLE)port->getNativeHandle() );
#elif SERIALPORT_POSIX
    if( port != nullptr && port->exists() ) tcdrain( port->getFileDescriptor() );
#endif
}

bool SerialPortOutputStream::write( const void* data, size_t numBytes )
{
#if SERIALPORT_WINDOWS
    if( port == nullptr || !port->exists() ) return false;
    auto*  p    = static_cast<const char*>( data );
    size_t left = numBytes;
    while( left > 0 )
    {
        const DWORD chunk = (DWORD)juce::jmin( left, (size_t)0x40000000 );
        DWORD       wrote = 0;
        if( !::WriteFile( (HANDLE)port->getNativeHandle(), p, chunk, &wrote, nullptr ) ) return false;
        if( wrote == 0 ) break;
        p += wrote;
        left -= wrote;
    }
    written += (juce::int64)numBytes;
    return true;
#elif SERIALPORT_POSIX
    if( port == nullptr || !port->exists() ) return false;
    auto*  p    = static_cast<const char*>( data );
    size_t left = numBytes;
    while( left > 0 )
    {
        const auto n = ::write( port->getFileDescriptor(), p, left );
        if( n < 0 )
        {
            if( errno == EAGAIN ) { juce::Thread::sleep( 1 ); continue; }
            return false;
        }
        p += n;
        left -= (size_t)n;
    }
    written += (juce::int64)numBytes;
    return true;
#else
    juce::ignoreUnused( data, numBytes );
    return false;
#endif
}
