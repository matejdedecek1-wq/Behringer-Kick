
namespace kick_ui
{
    SampleEditorPv::SampleEditorPv()
        : KickGrid( 4, 2, true )
    {
        setColumnWidth( { 354, -1 } );

        std::vector<juce::String> columTitle( 4 );
        columTitle[0] = "Layers";
        columTitle[1] = "Semi";
        columTitle[2] = "Fine";
        columTitle[3] = "Level";
        setRowTitles( columTitle );

        addComponent( layerOne, 0, 0, 334 );
        addComponent( layerTwo, 1, 0, 334 );
        addComponent( layerThree, 2, 0, 334 );
        addComponent( layerFour, 3, 0, 334 );

        addComponent( layerOnePan, 0, 1, 200 );
        addComponent( layerOneVelocityStart, 0, 1, 200 );
        addComponent( layerOneVelocityEnd, 0, 1, 200 );

        addComponent( layerTwoPan, 1, 1, 200 );
        addComponent( layerTwoVelocityStart, 1, 1, 200 );
        addComponent( layerTwoVelocityEnd, 1, 1, 200 );

        addComponent( layerThreePan, 2, 1, 200 );
        addComponent( layerThreeVelocityStart, 2, 1, 200 );
        addComponent( layerThreeVelocityEnd, 2, 1, 200 );

        addComponent( layerFourPan, 3, 1, 200 );
        addComponent( layerFourVelocityStart, 3, 1, 200 );
        addComponent( layerFourVelocityEnd, 3, 1, 200 );

        auto sampleFiles = mt::getAudioInterface().getAssets( mt::AssetTypes::Samples );

        layerOne.setListOptions( sampleFiles );
        layerTwo.setListOptions( sampleFiles );
        layerThree.setListOptions( sampleFiles );
        layerFour.setListOptions( sampleFiles );

        layerOnePan.setTitle( "" );
        layerOneVelocityStart.setTitle( "" );
        layerOneVelocityEnd.setTitle( "" );

        layerTwoPan.setTitle( "" );
        layerTwoVelocityStart.setTitle( "" );
        layerTwoVelocityEnd.setTitle( "" );

        layerThreePan.setTitle( "" );
        layerThreeVelocityStart.setTitle( "" );
        layerThreeVelocityEnd.setTitle( "" );

        layerFourPan.setTitle( "" );
        layerFourVelocityStart.setTitle( "" );
        layerFourVelocityEnd.setTitle( "" );
    }
}
