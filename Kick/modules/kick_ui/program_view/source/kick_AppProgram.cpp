
AppProgram::AppProgram()
    : kick_ui::IRowView( "Program View" )
{
    setupProvider();
    setMargin( 0, 6, 2, 0 );
}

AppProgram::~AppProgram()
{
    ae.removeListener( this );
}

void AppProgram::setupProvider()
{
    provider.numRows = [&]() -> int {
        return ae.getNumPrograms();
    };

    provider.cellComponent = [&]( int Row, Component* Existing ) -> juce::Component* {
        auto numProgs = ae.getNumPrograms();
        if( juce::isPositiveAndBelow( Row, numProgs ) )
        {
            auto comp = dynamic_cast<AppProgramComponent*>( Existing );
            if( comp == nullptr )
            {
                comp = new AppProgramComponent( Row );
            }
            comp->onPrgEdit = [&]() {
                if( onPrgEdit )
                    onPrgEdit();
            };
            comp->setProgram( Row );
            return comp;
        }
        return nullptr;
    };

    provider.onRowSelected = [&]( int Row ) {
        ae.setSelectedProgram( Row );
    };

    _initialized = true;
}

void AppProgram::populate( void )
{
    if( !isVisible() || !_initialized.load() )
        return;
    refresh();
}

void AppProgram::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
{
    if( contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
    {
        auto knobID    = contracts::DclContractsHelper::getKnobCode( f_HwMsg );
        auto knobValue = contracts::DclContractsHelper::getKnobValue( f_HwMsg );

        if( contracts::DeviceControlContracts::EN_KNOB_CENTER == knobID )
            IRowView::knobRotated( knobValue );
    }
}

//==============================================================================
void AppProgram::onProgramListChanged()
{
    if( ae.isProjectLoaded() )
        populate();
}

void AppProgram::onSelectionChange( const mt::MTSelected& Selected )
{
    selectRow( aeSel.program );
}

void AppProgram::visibilityChanged()
{
    IRowView::visibilityChanged();

    if( isVisible() )
        ae.addListener( this );
    else
        ae.removeListener( this );
}
