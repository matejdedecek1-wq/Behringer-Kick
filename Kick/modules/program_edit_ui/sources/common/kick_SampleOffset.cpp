
namespace kick_ui
{
    SampleOffset::SampleOffset()
        : KickGrid( 4, 2 )
    {
        setColumnWidth( { 354, -1 } );
        std::vector<juce::String> columTitle( 4 );
        columTitle[0] = "Layers";
        columTitle[1] = "Offset";
        setRowTitles( columTitle );

        addComponent( layerOne, 0, 0, 334 );
        addComponent( layerTwo, 1, 0, 334 );
        addComponent( layerThree, 2, 0, 334 );
        addComponent( layerFour, 3, 0, 334 );

        addComponent( off1, 0, 1, -1, 30 );
        addComponent( off2, 1, 1, -1, 30 );
        addComponent( off3, 2, 1, -1, 30 );
        addComponent( off4, 3, 1, -1, 30 );

        auto sampleFiles = mt::getAudioInterface().getAssets( mt::AssetTypes::Samples );

        layerOne.setListOptions( sampleFiles );
        layerTwo.setListOptions( sampleFiles );
        layerThree.setListOptions( sampleFiles );
        layerFour.setListOptions( sampleFiles );

        off1.setDisabled( true );
        off2.setDisabled( true );
        off3.setDisabled( true );
        off4.setDisabled( true );

        layerOne.onSelectionChanged = [this]( int, const juce::String& value ) {
            off1.setDisabled( value == "None" );
        };
        layerTwo.onSelectionChanged = [this]( int, const juce::String& value ) {
            off2.setDisabled( value == "None" );
        };
        layerThree.onSelectionChanged = [this]( int, const juce::String& value ) {
            off3.setDisabled( value == "None" );
        };
        layerFour.onSelectionChanged = [this]( int, const juce::String& value ) {
            off4.setDisabled( value == "None" );
        };
    }

    SampleOffset::~SampleOffset()
    {
    }

}
