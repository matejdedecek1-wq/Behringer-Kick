

juce::String CommonUtils::createID() noexcept
{
    const char   chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    juce::String id;
    juce::Random r;
    r.setSeedRandomly();

    id << chars[r.nextInt( 52 )]; // make sure the first character is always a letter
    for( int i = ( ID_LENGTH - 1 ); --i >= 0; )
    {
        id << chars[r.nextInt( 62 )];
    }
    return id;
}

float CommonUtils::dbToGain( float dbValue ) noexcept
{
    auto _fst = fpm::fast_pow<float>( 10.f, ( ( dbValue ) / 20.f ) );
    return _fst;
}

float CommonUtils::gainToDb( float gainValue ) noexcept
{
    constexpr float GAIN_THRESHOLD = 0.001;

    auto g = juce::jlimit( 0.f, 1.f, gainValue );
    if( GAIN_THRESHOLD < g )
    {
        return ( 20.f * fpm::fast_log10<float>( g ) );
    }
    return DB::MIN;
}

float CommonUtils::midiToDb( int MidiValue ) noexcept
{
    auto factor = (float)( MIDI::SCALER * MidiValue ); // (MidiLevel / 127 )
    return ( factor <= 0 ) ? DB::MIN : ( 40.0 * fpm::fast_log10<float>( factor ) );
}

int CommonUtils::dbToMidi( float dbValue ) noexcept
{
    auto factor = ( dbValue * 0.025f ); // (dbLevel / 40.f)
    return (int)( MIDI::MAX * fpm::fast_pow<float>( 10.0, factor ) );
}

float CommonUtils::midiToGain( int MidiValue ) noexcept
{
    return ( MIDI::SCALER * MidiValue );
}

int CommonUtils::gainToMidi( float Gain ) noexcept
{
    return (int)juce::jlimit( MIDI::MIN, MIDI::MAX, (int)( Gain * MIDI::MAX ) );
}

int CommonUtils::getDefaultMuteGroupForClipProgram( int Note ) noexcept
{
    auto note = juce::jlimit( MIDI::MIN, MIDI::MAX, Note );
    return ( ( ( note / 16 ) * 4 ) + ( note % 4 ) );
}

float CommonUtils::getPixelFromDb( const juce::NormalisableRange<double>& norRange, float displayRange, float dbValue, bool vertical )
{
    auto   db       = norRange.snapToLegalValue( dbValue );
    double relative = norRange.convertTo0to1( db );
    if( vertical )
        relative = 1.0 - relative;

    float pixel = (float)std::round( displayRange * relative );
    return pixel;
}

float CommonUtils::getDbFromPixel( const juce::NormalisableRange<double>& norRange, float displayRange, float pixel, bool vertical )
{
    double proportion = ( pixel - 0.0 ) / (double)displayRange;
    if( vertical )
        proportion = 1.0 - proportion;

    float dbValue = static_cast<float>( norRange.convertFrom0to1( proportion ) );
    return dbValue;
}

std::vector<juce::String> CommonUtils::getTimeDivAsString()
{
    std::vector<juce::String> notes;
    notes.push_back( "1/4" );
    notes.push_back( "1/8" );
    notes.push_back( "1/16" );
    notes.push_back( "1/32" );

    return notes;
}

std::vector<juce::String> CommonUtils::getDefaultTimeDivisions()
{
    std::vector<juce::String> notes;
    notes.push_back( "BAR" );
    notes.push_back( "1/4" );
    notes.push_back( "1/8" );
    notes.push_back( "1/16" );
    notes.push_back( "1/32" );
    notes.push_back( "1/64" );

    return notes;
}

std::vector<juce::String> CommonUtils::getTripletTimeDivisions()
{
    std::vector<juce::String> notes;
    notes.push_back( "BAR" );
    notes.push_back( "1/4T" );
    notes.push_back( "1/8T" );
    notes.push_back( "1/16T" );
    notes.push_back( "1/32T" );
    notes.push_back( "1/64T" );

    return notes;
}

bool CommonUtils::isValidTimeSig( int num, int den )
{
    if( num <= 16 && num > 0 && ( den == 4 || den == 8 || den == 16 || den == 32 ) )
        return true;

    return false;
}

int CommonUtils::timeDivStrToQl( const juce::String& timeDiv )
{
    static const std::unordered_map<juce::String, int> timeDivMap = {
        { "4", 4 }, { "8", 8 }, { "16", 16 }, { "32", 32 }, { "64", 64 }, { "4T", 6 }, { "8T", 12 }, { "16T", 24 }, { "32T", 48 }, { "64T", 96 }, { "BAR", 1 }, { "1/4", 4 }, { "1/8", 8 }, { "1/16", 16 }, { "1/32", 32 }, { "1/64", 64 }, { "1/4T", 6 }, { "1/8T", 12 }, { "1/16T", 24 }, { "1/32T", 48 }, { "1/64T", 96 }
    };

    auto it = timeDivMap.find( timeDiv );
    if( it != timeDivMap.end() )
        return it->second;

    return -1;
}

juce::String CommonUtils::getQLinkMode( const juce::ValueTree& vt )
{
    juce::ValueTree settings = vt.getChildWithName( juce::Identifier( "SETTINGS" ) );

    if( !settings.isValid() )
        return juce::String();

    juce::ValueTree qedit = settings.getChildWithName( juce::Identifier( "QLINKEDIT" ) );
    if( !qedit.isValid() )
        return juce::String();

    return qedit.getProperty( "qlinkmode" );
}

void CommonUtils::setQLinkMode( juce::ValueTree& vt, const juce::String& value )
{
    juce::ValueTree settings = vt.getChildWithName( juce::Identifier( "SETTINGS" ) );

    if( !settings.isValid() )
        return;

    juce::ValueTree qedit = settings.getChildWithName( juce::Identifier( "QLINKEDIT" ) );
    if( !qedit.isValid() )
        return;

    qedit.setProperty( "qlinkmode", value, nullptr );
}
