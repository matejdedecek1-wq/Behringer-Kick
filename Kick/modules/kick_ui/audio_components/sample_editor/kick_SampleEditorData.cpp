#include "kick_SampleEditorData.h"
namespace mt
{
    namespace ac
    {
        void Regions::update()
        {
            if( !state.isValid() )
                return;

            clear();
            if( state.getNumChildren() == 0 )
                state.setProperty( selectedIndexPropertyName, -1, nullptr );

            int currenSelIndex = state.getProperty( selectedIndexPropertyName );

            for( int i = 0; i < state.getNumChildren(); i++ )
            {
                juce::ValueTree regionState = state.getChild( i );
                if( regionState.getType() == juce::Identifier( "REGION_ALL" ) )
                {
                    allRegion = Region( regionState );
                    continue;
                }

                Region r( regionState );
                r.setIndex( i + 1 );
                Region& addedV = emplace_back( r );

                if( currenSelIndex != -1 )
                    seData->regionAdded( addedV );
            }

            if( state.getNumChildren() == 0 )
            {
                allRegion = Region( Region::createRegion( 0, seData->getSampleLengthInSamples(), nullptr, true ) );
                allRegion.setIndex( -1 );
                state.addChild( allRegion.getState(), 0, nullptr );
            }
            if( currenSelIndex == -1 )
                seData->regionAdded( allRegion );

            selectRegion( currenSelIndex );
        }
        void Regions::setSedata( SampleEditorData* d )
        {
            seData = d;
            state.addListener( this );
        }

        void Regions::valueTreePropertyChanged( juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property )
        {
            //regions is the odd place for checking this property need to move it to SampleEditor data
            if( treeWhosePropertyHasChanged.getType() == juce::Identifier( "SAMPLESTATE" ) && property == juce::Identifier( "mode" ) )
            {
                int mode = treeWhosePropertyHasChanged.getProperty( property );
                seData->setSplitMode( (SampleEditorData::SplitMode)mode );
                ensureSelection();
            }
            else if( treeWhosePropertyHasChanged.getType() == juce::Identifier( "SAMPLESTATE" ) && property == juce::Identifier( selectedIndexPropertyName ) )
            {
                int currSel = treeWhosePropertyHasChanged.getProperty( property );
                selectRegion( currSel );
            }
            else if( treeWhosePropertyHasChanged.getType() == juce::Identifier( "REGION" ) )
            {
                for( auto it = begin(); it != end(); it++ )
                {
                    if( treeWhosePropertyHasChanged == it->getState() )
                    {
                        it->updateValues();
                        seData->regionChanged( *it );
                    }
                }
            }
            else if( treeWhosePropertyHasChanged.getType() == juce::Identifier( "REGION_ALL" ) )
            {
                if( treeWhosePropertyHasChanged == allRegion.getState() )
                {
                    allRegion.updateValues();
                    seData->regionChanged( allRegion );
                }
            }
        }

        void Regions::valueTreeChildRemoved( juce::ValueTree& /*parentTree*/, juce::ValueTree& /*childWhichHasBeenRemoved*/, int indexFromWhichChildWasRemoved )
        {
            int index = 0;
            for( auto it = begin(); it != end(); it++, index++ )
            {
                if( index == indexFromWhichChildWasRemoved )
                {
                    Region delRegion = *it;

                    seData->regionAboutToBeRemoved( delRegion );
                    std::list<Region>::erase( it );
                    seData->regionRemoved( delRegion );

                    ensureSelection();
                    break;
                }
            }
        }

        void Regions::valueTreeChildAdded( juce::ValueTree& /*parentTree*/, juce::ValueTree& childWhichHasBeenAdded )
        {
            if( ignoreValueTreeChanges )
            {
                ensureSelection();
                return;
            }

            if( childWhichHasBeenAdded.getType() == juce::Identifier( "REGION_ALL" ) )
                return;

            Region region( childWhichHasBeenAdded );
            auto   it = begin();

            for( ; it != end(); it++ )
            {
                if( it->getStart() > region.getStart() )
                    break;
            }

            auto newIt = std::list<Region>::insert( it, region );
            int  index = 0;
            index      = static_cast<int>( std::distance( begin(), newIt ) );
            for( auto updateIt = newIt; updateIt != end(); updateIt++ )
            {
                updateIt->setIndex( index + 1 );
                index++;
            }
            seData->regionAdded( *newIt );
            ensureSelection();
        }

        std::list<Region>::iterator Regions::erase( std::list<Region>::iterator it )
        {
            // note value removed from list in valueTreeChildRemoved
            int index = static_cast<int>( std::distance( begin(), it ) );
            state.removeChild( index, &undoManager );
            return std::list<Region>::iterator();
        }

        std::list<Region>::iterator Regions::insert( std::list<Region>::iterator it, Region& region )
        {
            int index              = static_cast<int>( std::distance( begin(), it ) );
            ignoreValueTreeChanges = true;

            auto newIt = std::list<Region>::insert( it, region );
            state.addChild( region.getState(), index, &undoManager );

            for( auto updateIt = newIt; updateIt != end(); updateIt++ )
            {
                updateIt->setIndex( index + 1 );
                index++;
            }

            seData->regionAdded( ( *newIt ) );
            ignoreValueTreeChanges = false;

            return newIt;
        }

        void Regions::createEqualRegions( unsigned int slices )
        {
            if( !state.isValid() )
                return;
            juce::int64 length    = ( allRegion.getEnd() - allRegion.getStart() ) / slices;
            juce::int64 tempStart = allRegion.getStart();
            juce::int64 tempEnd   = length;
            if( size() < slices )
            {
                size_t noToAdd = slices - size();
                tempStart += length * (juce::int64)size();
                tempEnd = tempStart + length;
                for( unsigned int i = 0; i < noToAdd; i++ )
                {
                    Region newR( Region::createRegion( tempStart, tempEnd, nullptr ) );
                    insert( end(), newR );
                    tempStart += length;
                    tempEnd += length;
                }
            }
            while( size() > slices )
            {
                Region delRegion = back();
                erase( std::next( end(), -1 ) );
            }

            tempStart = allRegion.getStart();
            tempEnd   = tempStart + length;
            for( auto it = begin(); it != end(); it++ )
            {
                if( it->getStart() != tempStart )
                    it->setStart( tempStart, &undoManager );
                if( it->getEnd() != tempEnd )
                    it->setEnd( tempEnd, &undoManager );

                tempStart += length;
                tempEnd += length;
                seData->regionChanged( *it );
            }
        }

        const Region& Regions::getRegion( int pos )
        {
            int i = 0;
            for( auto it = begin(); it != end(); it++ )
            {
                if( i == pos )
                    return *it;

                i++;
            }

            return invalidRegion;
        }

        const Region& Regions::getSelectedRegion() const
        {
            if( allRegion.isSelected() )
                return allRegion;

            for( auto it = begin(); it != end(); it++ )
                if( it->isSelected() )
                    return *it;

            return invalidRegion;
        }

        void Regions::loopSelectedRegion() {}

        void Regions::pop_back()
        {
            if( empty() )
                return;

            size_t lastIndex = size() - 1;
            state.removeChild( (int)lastIndex, nullptr );
        }

        void Regions::setSelectedRegionLoopMode( LoopMode loopMode )
        {
            const_cast<Region&>( getSelectedRegion() ).setLoopMode( loopMode, &undoManager );
        }

        LoopMode Regions::getSelectedRegionLoopMode() const
        {
            return getSelectedRegion().getLoopMode();
        }

        void Regions::joinRegions()
        {
            if( size() == 1 )
                return;

            for( auto it = begin(); it != end(); it++ )
            {
                if( it->isSelected() )
                {
                    auto region = it;
                    undoManager.beginNewTransaction();
                    if( it != begin() )
                    {
                        auto prev = std::prev( it );
                        prev->setEnd( it->getEnd(), &undoManager );
                        region = prev;
                    }
                    else
                    {
                        auto next = std::next( it );
                        if( next != end() )
                        {
                            next->setStart( it->getStart(), &undoManager );
                            region = next;
                        }
                    }
                    erase( it );
                    updateIndex();
                    region->setSelected( true );
                    seData->regionChanged( *region );
                    break;
                }
            }
        }

        Region& Regions::splitAtTime( juce::int64 start, bool selectNewRegion )
        {
            auto it = begin();
            for( ; it != end(); it++ )
            {
                if( it->getStart() > start )
                {
                    break;
                }
            }

            juce::int64 end = seData->getSampleLengthInSamples();
            findEnd( end, start, it );
            Region region( Region::createRegion( start, end, nullptr ) );

            it = insert( it, region );
            if( selectNewRegion )
            {
                selectRegion( ( *it ) );
            }

            return ( *it );
        }

        void Regions::selectRegion( const Region& regionToSelect )
        {
            int currentIndex = selectedIndex;

            if( regionToSelect == allRegion )
            {
                selectedIndex = -1;
                if( !allRegion.isSelected() )
                {
                    allRegion.setSelected( true );
                    seData->regionAdded( allRegion );
                }
                else
                {
                    return;
                }
            }
            else
            {
                allRegion.setSelected( false );
                for( auto it = begin(); it != end(); it++ )
                {
                    if( *it == regionToSelect )
                    {
                        /*if( it->isSelected() )
                            return;*/

                        ( *it ).setSelected( true );
                        selectedIndex = static_cast<int>( std::distance( begin(), it ) );
                    }
                    else
                    {
                        ( *it ).setSelected( false );
                    }
                }
            }

            if( currentIndex != selectedIndex )
            {
                if( selectedIndex == -1 && currentIndex > -1 )
                {
                    for( auto it = begin(); it != end(); it++ )
                    {
                        seData->regionAboutToBeRemoved( *it );
                        seData->regionRemoved( *it );
                    }
                    seData->regionAdded( allRegion );
                }
                else if( selectedIndex > -1 && currentIndex == -1 )
                {
                    for( auto it = begin(); it != end(); it++ )
                        seData->regionAdded( *it );

                    seData->regionAboutToBeRemoved( allRegion );
                    seData->regionRemoved( allRegion );
                }

                if( !undoManager.isPerformingUndoRedo() )
                    state.setProperty( selectedIndexPropertyName, selectedIndex, &undoManager );
                else
                    selectedIndexDirty = true;
            }

            seData->regionChanged( regionToSelect );
        }

        void Regions::selectRegion( const int regionIndex )
        {
            if( !state.isValid() )
                return;
            if( regionIndex == -1 )
            {
                selectRegion( allRegion );
                seData->regionSelected( allRegion );
            }
            else
            {
                int i = 0;
                for( auto it = begin(); it != end(); it++ )
                {
                    if( i == regionIndex )
                    {
                        selectRegion( *it );
                        seData->regionSelected( *it );
                    }

                    i++;
                }
            }
        }

        void Regions::splitSelectedRegion()
        {
            for( auto it = begin(); it != end(); it++ )
            {
                if( it->isSelected() )
                {
                    undoManager.beginNewTransaction();
                    juce::int64 half = static_cast<juce::int64>( std::round( ( it->getEnd() - it->getStart() ) / 2 ) );
                    Region      newRegion( Region::createRegion( it->getStart() + half, it->getEnd(), nullptr ) );
                    it->setEnd( it->getStart() + half, &undoManager );
                    it = std::next( it );
                    it = insert( it, newRegion );
                    break;
                }
            }
        }

        void Regions::deleteRegion( const Region& region )
        {
            for( auto it = begin(); it != end(); it++ )
            {
                if( region == ( *it ) )
                {
                    undoManager.beginNewTransaction();
                    erase( it );
                    updateIndex();
                    break;
                }
            }
        }

        void Regions::clearAllRegions()
        {
            while( size() )
            {
                erase( begin() );
            }
        }

        void Regions::setLoopPos( juce::int64 pos, bool commit )
        {
            Region& selRegion = const_cast<Region&>( getSelectedRegion() );
            if( selRegion.isValid() )
            {
                if( commit )
                    undoManager.beginNewTransaction();

                if( commit )
                    selRegion.setLoopPos( pos, &undoManager );
                else
                    selRegion.setLoopPos( pos, nullptr );

                seData->loopPositionChanged( pos );
            }
        }

        juce::int64 Regions::getLoopPosition() const
        {
            return getSelectedRegion().getLoopPos();
        }

        void Regions::ensureSelection()
        {
            int selectIndexValue = state.getProperty( selectedIndexPropertyName );
            if( (unsigned int)selectIndexValue >= size() && selectIndexValue != -1 )
                selectIndexValue = static_cast<int>( size() - 1 );
            else if( selectIndexValue == -1 && size() )
                selectIndexValue = 0;

            selectRegion( selectIndexValue );
        }

        void Regions::changeListenerCallback( juce::ChangeBroadcaster* source )
        {
            if( source == &undoManager && selectedIndexDirty )
            {
                selectedIndexDirty = false;
                state.setProperty( selectedIndexPropertyName, selectedIndex, &undoManager );
            }
        }

        size_t Regions::size()
        {
            return std::list<Region>::size();
        }

        int Regions::getSelectedIndex()
        {
            if( selectedIndexDirty )
                return selectedIndex;
            else
                return state.getProperty( selectedIndexPropertyName );
        }

        void Regions::updateIndex()
        {
            int index = 1;
            for( auto it = begin(); it != end(); it++ )
            {
                it->setIndex( index );
                index++;
            }
        }

        void Regions::findEnd( juce::int64& regionEnd, juce::int64 start, std::list<Region>::iterator& it )
        {
            if( it == begin() )
                return;

            auto prev = std::prev( it );
            if( start > prev->getEnd() )
            {
                if( it != end() )
                {
                    regionEnd = it->getStart();
                }
            }
            else
            {
                regionEnd = prev->getEnd();
                prev->setEnd( start, &undoManager );
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// //////////////////////SampleEditorData
        /////////////////////////////////////////////////////////////////////////
        SampleEditorData::SampleEditorData()
            : undoManager( getAudioInterface().getUndoManager() )
        {
            supportedActions.push_back( "Discard" );
            supportedActions.push_back( "Delete" );
            supportedActions.push_back( "Silence" );
            supportedActions.push_back( "Normalize" );
            supportedActions.push_back( "Reverse" );
            supportedActions.push_back( "FadeIn" );
            supportedActions.push_back( "FadeOut" );
            supportedActions.push_back( "Extract" );
            supportedActions.push_back( "PitchShift" );
            supportedActions.push_back( "Time Stretch" );
            supportedActions.push_back( "Gain Change" );
            supportedActions.push_back( "Copy" );
            supportedActions.push_back( "Bit Reduce" );
            supportedActions.push_back( "Stereo > Mono" );

            regions.setSedata( this );

            createValueTreeItem();

            sei = getAudioInterface().createSampleEditor();
            sei->addListener( this );
        }

        SampleEditorData::~SampleEditorData()
        {
            if( sei )
                delete sei;
        }

        juce::AudioThumbnailBase* SampleEditorData::getThumbnail()
        {
            if( !sei )
                return nullptr;

            return sei->getThumbnail();
        }

        void SampleEditorData::initialize( const juce::String& audioSamplePath )
        {
            currentSamplePath = audioSamplePath;
            if( currentSamplePath.isEmpty() )
            {
                currentSamplePath = ""; //getLayer(type);
            }
            else
            {
                createValueTreeItem();
                //setLayer(currentSamplePath, type);
            }
            juce::File file( currentSamplePath );
            sampleAboutToBeChanged();

            sei->setSource( file );
            bpm = sei->detectBpm();

            //calculate no beats
            double len = sei->getLengthInSec() / 60;
            totalBeats = ( len * bpm );

            auto editor = dynamic_cast<SEViewComponent*>( view );
            editor->resetZoomFactor();
            //create stored regions
            regions.update();
            sampleChanged();
        }

        double SampleEditorData::getTotalBeats()
        {
            if( totalBeats == 0 )
            {
                double len = sei->getLengthInSec() / 60;
                totalBeats = ( len * bpm );
            }

            return totalBeats;
        }

        void SampleEditorData::createValueTreeItem()
        {
            if( !sampleEditorState.isValid() )
            {
                juce::ValueTree& vTree = getAudioInterface().getValueTree();
                sampleEditorState      = vTree.getChildWithName( juce::Identifier( "SAMPLEEDITOR" ) );

                if( !sampleEditorState.isValid() )
                {
                    sampleEditorState = juce::ValueTree( juce::Identifier( "SAMPLEEDITOR" ) );
                    vTree.appendChild( sampleEditorState, nullptr );
                }

                juce::String timeLineUits = sampleEditorState.getProperty( seSettingsTuPropertyName );
                juce::String cPlay        = sampleEditorState.getProperty( seSettingsCuePlay );
                juce::String cPreview     = sampleEditorState.getProperty( seSettingsCuePreview );
                juce::String sPreview     = sampleEditorState.getProperty( seSettingsSlicePreview );
                juce::String aScroll      = sampleEditorState.getProperty( seSettingsAutoScroll );

                if( timeLineUits == timelineIdBeats )
                    tUnits = TimeLineUnits::unitsBeats;
                else if( timeLineUits == timelineIdSamples )
                    tUnits = TimeLineUnits::unitsSamples;
                else if( timeLineUits == timelineIdTime )
                    tUnits = TimeLineUnits::unitsTime;

                if( cPlay == cuePlayIdToggle )
                    cuePlay = CuePlay::cuePlayToggle;
                else if( cPlay == cuePlayIdOneshot )
                    cuePlay = CuePlay::cuePlayOneShot;

                if( cPreview == cuePreviewIdAfter )
                    cuePreview = CuePreview::cuePreviewAfter;
                else if( cPreview == cuePreviewIdBefore )
                    cuePreview = CuePreview::cuePreviewBefore;
                else
                    cuePreview = CuePreview::cuePreviewOff;

                if( sPreview == slicePreviewIdBefore )
                    slicePreview = SlicePreview::slicePreviewBefore;
                else if( sPreview == slicePreviewIdAfter )
                    slicePreview = SlicePreview::slicePreviewAfter;
                else
                    slicePreview = SlicePreview::slicePreviewOff;

                if( aScroll == autoScrollIdFollow )
                    autoScroll = AutoScroll::autoScrollFollow;
                else if( aScroll == autoScrollIdPage )
                    autoScroll = AutoScroll::autoScrollPage;
                else
                    autoScroll = AutoScroll::autoScrollOff;
            }
            if( currentSamplePath.isNotEmpty() )
            {
                sampleState = sampleEditorState.getChildWithProperty( juce::Identifier( "name" ), currentSamplePath );
                if( !sampleState.isValid() )
                {
                    sampleState = juce::ValueTree( "SAMPLESTATE" );
                    sampleState.setProperty( "name", currentSamplePath, nullptr );
                    sampleState.setProperty( "mode", (int)SplitMode::splitModeManual, nullptr );
                    sampleState.setProperty( loopPosPropertyId, 0.0f, nullptr );
                    sampleStateLayer1 = sampleState;

                    regionState = juce::ValueTree( "REGIONS" );

                    sampleState.appendChild( regionState, nullptr );
                    sampleEditorState.appendChild( sampleState, nullptr );
                }
                else
                {
                    regionState = sampleState.getChildWithName( "REGIONS" );
                }

                // initialize Settings
                int mode  = sampleState.getProperty( "mode" );
                splitMode = (SplitMode)mode;
            }
        }

        Region& SampleEditorData::splitAtTime( juce::int64 start, bool selectNewRegion )
        {
            juce::int64 totalLength = getSampleLengthInSamples();
            if( start < 0 || start > totalLength /* || isTrimMode()*/ )
                return invalidRange;

            undoManager.beginNewTransaction();
            return regions.splitAtTime( start, selectNewRegion );
        }

        Region& SampleEditorData::getLastRegion()
        {
            if( regions.size() )
                return regions.back();

            return invalidRange;
        }

        void SampleEditorData::splitByThreshImp( std::vector<juce::var>& params )
        {
            regions.clearAllRegions();
            int                    thresh      = params[0];
            int                    mintime     = params[1];
            std::list<juce::int64> splitValues = sei->splitByThreshold( thresh, mintime, regions.getAllRegion().getStart(), regions.getAllRegion().getEnd() );
            if( splitValues.size() )
                for( juce::int64 splitTime : splitValues )
                    regions.splitAtTime( splitTime, false );
            else
                regions.splitAtTime( 0, false );
        }

        void SampleEditorData::linkSlicesImp()
        {
            for( auto it = regions.begin(); it != regions.end(); it++ )
            {
                juce::int64 nextStart, nextEnd, prevStart, prevEnd = 0;
                getNextStartAndEnd( it, nextStart, nextEnd );
                getPrevStartAndEnd( it, prevStart, prevEnd );
                /*if (it->getStart() > nextStart)
                {
                    it->getStart() = prevStart;
                    it->getEnd() = nextStart;
                }
                else if (it->getStart() < prevEnd)
                {
                    it->getStart() = prevEnd;
                    it->getEnd() = nextStart;
                }
                else*/
                {
                    it->setStart( prevEnd, &undoManager );
                    it->setEnd( nextStart, &undoManager );
                }
                regionChanged( *it );
            }
        }

        bool SampleEditorData::moveTime( const Region& range, juce::int64 newTime, bool end, bool commit )
        {
            juce::int64 length = getSampleLengthInSamples();
            if( ( !end && range.getEnd() < newTime ) || ( end && range.getStart() > newTime ) || ( end && newTime <= range.getStart() ) )
                return false;

            if( ( end && newTime > length ) || ( !end && newTime < 0 ) )
                return false;

            // case move all regions
            if( range == regions.getAllRegion() )
            {
                juce::UndoManager* undoMgr = nullptr;
                if( commit )
                    undoMgr = &undoManager;

                undoManager.beginNewTransaction();
                Region& allRegion = regions.getAllRegion();
                if( end )
                    allRegion.setEnd( newTime, undoMgr );
                else
                    allRegion.setStart( newTime, undoMgr );

                regionChanged( range );
                return true;
            }

            auto it = std::find( regions.begin(), regions.end(), range );

            juce::int64 prevStart, prevEnd = 0;
            getPrevStartAndEnd( it, prevStart, prevEnd );
            if( getLinkSlices() && newTime < prevStart )
                return false;

            // case not found
            if( it == regions.end() )
                return false;
            juce::UndoManager* undoMgr = nullptr;
            if( commit )
                undoMgr = &undoManager;

            undoManager.beginNewTransaction();
            if( end )
                ( *it ).setEnd( newTime, undoMgr );
            else
                ( *it ).setStart( newTime, undoMgr );

            if( !getLinkSlices() )
            {
                regionChanged( range );
                return true;
            }
            // case resizing last end
            auto dist = static_cast<size_t>( std::distance( regions.begin(), it ) );
            if( end && dist == regions.size() - 1 )
            {
                regionChanged( range );
                return true;
            }
            auto bef = regions.end();
            if( it != regions.begin() )
                bef = std::prev( it );
            if( bef != regions.end() && !end )

            {
                if( bef->getStart() > newTime )
                    return false;

                bef->setEnd( newTime, undoMgr );
            }

            auto next = std::next( it );
            if( next != regions.end() && end )
            {
                if( next->getEnd() < newTime )
                    return false;

                next->setStart( newTime, undoMgr );
            }

            regionChanged( range );
            regionSelected( range );
            return true;
        }

        void SampleEditorData::selectRegion( int index )
        {
            undoManager.beginNewTransaction();
            regions.selectRegion( index );
        }

        void SampleEditorData::selectRegion( const Region& regionToSelect )
        {
            undoManager.beginNewTransaction();
            regions.selectRegion( regionToSelect );
        }

        void SampleEditorData::clearAllRegions()
        {
            undoManager.beginNewTransaction();
            regions.clearAllRegions();
        }

        double SampleEditorData::getSampleLength()
        {
            if( !sei )
                return 0;

            return sei->getLengthInSec();
        }

        juce::int64 SampleEditorData::getSampleLengthInSamples()
        {
            if( !sei )
                return 0;

            return sei->getLengthInSamples();
        }

        bool SampleEditorData::isEnd( const Region& region )
        {
            auto it = std::find( regions.begin(), regions.end(), region );
            return ( it == regions.end() || std::next( it ) == regions.end() );
        }

        void SampleEditorData::splitSelectedRegion()
        {
            regions.splitSelectedRegion();
        }

        void SampleEditorData::splitAtCuePos()
        {
            if( playHeadPos != -1 )
            {
                regions.splitAtTime( playHeadPos, true );
                setCuePosition( playHeadPos );
            }
            else
            {
                regions.splitAtTime( getCuePosition(), true );
            }
        }

        void SampleEditorData::joinSelectedAndPrev()
        {
            regions.joinRegions();
        }

        int SampleEditorData::getPosition( const Region& region )
        {
            for( auto it = regions.begin(); it != regions.end(); it++ )
            {
                if( region == ( *it ) )
                    return static_cast<int>( std::distance( regions.begin(), it ) );
            }

            return -1;
        }

        void SampleEditorData::removeLastRegion()
        {
            regions.pop_back();
        }

        void SampleEditorData::setLinkSlices( bool link )
        {
            if( link == linkSlices )
                return;

            linkSlices = link;
            if( linkSlices )
            {
                linkSlicesImp();
            }

            linkSlicesChanged( linkSlices );
        }

        const std::vector<juce::String>& SampleEditorData::getSampleList()
        {
            return getAudioInterface().getAssets( AssetTypes::Samples );
        }

        juce::int64 SampleEditorData::convertTimeToSamples( double time )
        {
            if( !sei )
                return 0;

            juce::int64 totalLengthInSample = sei->getLengthInSamples();
            double      totalLengthInTime   = sei->getLengthInSec();
            juce::int64 samples             = static_cast<juce::int64>( std::round( ( time * totalLengthInSample ) / totalLengthInTime ) );
            return samples;
        }

        double SampleEditorData::convertSamplesToTime( juce::int64 sample )
        {
            juce::int64 totalLengthInSample = sei->getLengthInSamples();
            double      totalLengthInTime   = sei->getLengthInSec();

            double time = ( sample * totalLengthInTime ) / totalLengthInSample;
            return time;
        }

        void SampleEditorData::setMode( SampleEditorData::Mode newMode )
        {
            editMode = newMode;
            modeChanged( editMode );
        }

        bool SampleEditorData::isTrimMode()
        {
            return ( editMode == Mode::modeTrim );
        }

        bool SampleEditorData::isChopMode()
        {
            bool isChop = ( editMode == Mode::modeChop );
            return isChop;
        }

        float SampleEditorData::getPosInPixels( juce::int64 posTime )
        {
            juce::int64 length = getSampleLengthInSamples();
            if( length )
            {
                float posInPixels = ( view->getWidth() * posTime * 1.0f ) / length;
                return posInPixels;
            }

            return -1;
        }

        juce::int64 SampleEditorData::getPosInSeconds( float posInPix )
        {
            juce::int64 length = getSampleLengthInSamples();
            juce::int64 value  = static_cast<juce::int64>( std::round( ( posInPix * length ) / view->getLocalBounds().getWidth() ) );
            return value;
        }

        void SampleEditorData::setCuePosition( juce::int64 newPosition )
        {
            if( cuePosition != newPosition )
            {
                cuePosition = newPosition;
                cuePositionChanged( newPosition );
            }
        }

        void SampleEditorData::setLoopPosition( juce::int64 pos, bool commit )
        {
            regions.setLoopPos( pos, commit );
        }

        juce::int64 SampleEditorData::getLoopPosition()
        {
            return regions.getLoopPosition();
        }

        juce::int64 SampleEditorData::getCuePosition()
        {
            return cuePosition;
        }

        void SampleEditorData::setSplitMode( SplitMode newSplitMode )
        {
            if( splitMode == newSplitMode || !sampleState.isValid() )
                return;

            splitMode         = newSplitMode;
            int  mode         = sampleState.getProperty( "mode" );
            auto splitModeTmp = (SplitMode)mode;
            if( splitModeTmp != splitMode )
            {
                undoManager.beginNewTransaction();
                sampleState.setProperty( "mode", (int)splitMode, &undoManager );
            }

            splitModeChanged( splitMode );
        }

        void SampleEditorData::setSplitMode( SplitMode newSplitMode, std::vector<juce::var>& params )
        {
            if( splitMode == newSplitMode )
                return;

            splitMode = newSplitMode;
            undoManager.beginNewTransaction();
            sampleState.setProperty( "mode", (int)splitMode, &undoManager );
            if( splitMode == SplitMode::splitModeRegions )
            {
                int noSlices = *params.begin();
                regions.createEqualRegions( (unsigned int)noSlices );
            }
            else if( splitMode == SplitMode::splitModeThreshold )
            {
                splitByThreshImp( params );
            }
            else if( splitMode == SplitMode::splitModeBpm )
            {
                splitByBpmImp( params );
            }
        }

        void SampleEditorData::splitByBpm( std::vector<juce::var>& params )
        {
            undoManager.beginNewTransaction();
            splitByBpmImp( params );
        }

        void SampleEditorData::splitByThresh( std::vector<juce::var>& params )
        {
            if( getSpliMode() != SplitMode::splitModeThreshold )
                return;

            splitByThreshImp( params );
        }

        void SampleEditorData::splitByBpmImp( std::vector<juce::var>& params )
        {
            if( getSpliMode() != SplitMode::splitModeBpm )
                return;

            int bars                = params[0];
            int beats               = params[1];
            int timeSig             = CommonUtils::timeDivStrToQl( params[2] );
            int barDivisionFraction = timeSig / 4;
            int totalDivisions      = bars * ( beats * barDivisionFraction );
            regions.createEqualRegions( (unsigned int)totalDivisions );
        }

        SampleEditorData::SplitMode SampleEditorData::getSpliMode()
        {
            if( isChopMode() )
            {
                return splitMode;
            }
            else
            {
                return SplitMode::splitModeNone;
            }
        }

        bool SampleEditorData::isListenTool()
        {
            return ( selectedTool == SeTool::seToolListen );
        }

        bool SampleEditorData::isSplitModeManual()
        {
            if( isChopMode() && splitMode == SplitMode::splitModeManual && selectedTool == SeTool::seToolEdit )
                return true;

            return false;
        }

        void SampleEditorData::setNoSlices( unsigned int slices )
        {
            undoManager.beginNewTransaction();
            /*if( splitMode != SplitMode::splitModeRegions )
                return;*/

            regions.createEqualRegions( slices );
        }

        void SampleEditorData::performAction( unsigned int actionId, const std::list<juce::var>& params )
        {
            const Region& selRegion = getSelectedRegion();
            if( !selRegion.isValid() )
            {
                jassertfalse; //there should always be a valid selection
                return;
            }

            MTSampleRegion smpRegion( selRegion.getStart(), selRegion.getEnd() );
            sei->performAction( smpRegion, MTSampleEditorAction( actionId ), params );
        }

        juce::int64 SampleEditorData::getNextZeroSnapPos( juce::int64 pos, bool forward )
        {
            return sei->getNextZeroSnapPos( pos, forward );
        }

        void SampleEditorData::setTimeLineUnits( TimeLineUnits units )
        {
            juce::String propertyName;
            if( units == TimeLineUnits::unitsBeats )
                propertyName = timelineIdBeats;
            else if( units == TimeLineUnits::unitsSamples )
                propertyName = timelineIdSamples;
            else
                propertyName = timelineIdTime;

            sampleEditorState.setProperty( "TimeLineUnits", propertyName, nullptr );
            tUnits = units;
            settingsChanged();
        }

        const Region& SampleEditorData::getSelectedRegion()
        {
            return regions.getSelectedRegion();
        }

        void SampleEditorData::deleteRegion( const Region& region )
        {
            if( getSpliMode() != SplitMode::splitModeManual )
            {
                setSplitMode( SplitMode::splitModeManual );
            }
            regions.deleteRegion( region );
        }

        void SampleEditorData::setLoopMode( LoopMode newLoopMode )
        {
            regions.setSelectedRegionLoopMode( newLoopMode );
        }

        void SampleEditorData::setAutoScroll( AutoScroll newScroll )
        {
            juce::String propertyName;
            if( newScroll == AutoScroll::autoScrollFollow )
                propertyName = autoScrollIdFollow;
            else if( newScroll == AutoScroll::autoScrollOff )
                propertyName = autoScrollIdOff;
            else
                propertyName = autoScrollIdPage;

            sampleEditorState.setProperty( "TimeLineUnits", propertyName, nullptr );
            autoScroll = newScroll;
            settingsChanged();
        }

        std::vector<unsigned int> SampleEditorData::getDisabledAction()
        {
            if( getSelectedRegion().getIndex() != -1 )
                return { seActionDiscard, seActionDelete, seActionTimeStretch, seActionCopy, seActionBitReduce, seActionStereoToMono };
            else
                return {};
        }

        double SampleEditorData::getNumberOfBeats()
        {
            if( sei )
                return sei->getNumberOfBeats();

            return 0;
        }

        void SampleEditorData::setCuePreview( CuePreview cPreview )
        {
            if( cPreview == cuePreview )
                return;

            cuePreview = cPreview;
            juce::String propertyValue;
            if( cuePreview == CuePreview::cuePreviewOff )
                propertyValue = cuePreviewIdOff;
            if( cuePreview == CuePreview::cuePreviewAfter )
                propertyValue = cuePreviewIdAfter;
            if( cuePreview == CuePreview::cuePreviewBefore )
                propertyValue = cuePreviewIdBefore;

            sampleEditorState.setProperty( seSettingsCuePreview, propertyValue, nullptr );
        }

        void SampleEditorData::setSlicePreview( SlicePreview sPreview )
        {
            if( sPreview == slicePreview )
                return;

            slicePreview = sPreview;
            juce::String propertyValue;
            if( slicePreview == SlicePreview::slicePreviewOff )
                propertyValue = slicePreviewIdOff;
            if( slicePreview == SlicePreview::slicePreviewAfter )
                propertyValue = slicePreviewIdAfter;
            if( slicePreview == SlicePreview::slicePreviewBefore )
                propertyValue = slicePreviewIdBefore;

            sampleEditorState.setProperty( seSettingsSlicePreview, propertyValue, nullptr );
        }

        void SampleEditorData::undo()
        {
            sei->undo();
        }

        void SampleEditorData::redo()
        {
            sei->redo();
        }

        void SampleEditorData::playSelectedRegion( MTSamplePlayMode Mode )
        {
            const auto& selReg = getSelectedRegion();
            play( selReg, Mode );
        }

        void SampleEditorData::play( int slice )
        {
            if( slice == -1 )
                sei->play();
            isPlaying            = true;
            const Region& region = regions.getRegion( slice );
            play( region );
        }

        void SampleEditorData::playLoop()
        {
            Region   region   = regions.getSelectedRegion();
            LoopMode loopMode = region.getLoopMode();
            if( loopMode == LoopMode::loopModeNone )
                return;

            if( !getLoopLock() )
                region.setStart( region.getLoopPos(), nullptr );

            MTSamplePlayMode samplePlayMode = MTSamplePlayMode::samplePlayModeLoopForward;

            if( loopMode == LoopMode::loopModeForward )
                samplePlayMode = MTSamplePlayMode::samplePlayModeLoopForward;
            else if( loopMode == LoopMode::loopModeBidirection )
                samplePlayMode = MTSamplePlayMode::samplePlayModeLoopBidirectional;
            else if( loopMode == LoopMode::loopModeBackward )
                samplePlayMode = MTSamplePlayMode::samplePlayModeLoopBackward;
            else
                jassertfalse;

            play( region, samplePlayMode );
        }

        void SampleEditorData::play( const Region& region, MTSamplePlayMode mode )
        {
            if( !region.isValid() )
                return;
            isPlaying = true;
            MTSampleRegion mtRegion( region.getStart(), region.getEnd() );
            sei->play( mtRegion, mode );
        }

        void SampleEditorData::play( const MTSampleRegion& region, MTSamplePlayMode mode )
        {
            isPlaying = true;
            sei->play( region, mode );
        }

        void SampleEditorData::playCue()
        {
            MTSampleRegion sampleRegion( getCuePosition(), getSampleLengthInSamples() );
            sei->play( sampleRegion, MTSamplePlayMode::samplePlayFromStart );
        }

        void SampleEditorData::stop()
        {
            isPlaying = false;
            sei->stop();
        }

        void SampleEditorData::positionChanged( juce::int64 pos )
        {
            playHeadPos = pos;
            playHeadChanged( pos );
        }

        void SampleEditorData::stopped()
        {
            isPlaying   = false;
            playHeadPos = -1;
            playbackStopped();
        }

        void SampleEditorData::sampleModified()
        {
            currentSampleModified();
        }

        void SampleEditorData::getNextStartAndEnd( std::list<Region>::iterator& it, juce::int64& nextStart, juce::int64& nextEnd )
        {
            auto next = std::next( it );
            if( next == regions.end() )
            {
                nextStart = getSampleLengthInSamples();
                nextEnd   = nextStart;
            }
            else
            {
                nextStart = next->getStart();
                nextEnd   = next->getEnd();
            }
        }

        void SampleEditorData::getPrevStartAndEnd( std::list<Region>::iterator& it, juce::int64& prevStart, juce::int64& prevEnd )
        {
            if( it == regions.begin() )
            {
                prevStart = 0;
                prevEnd   = 0;
            }
            else
            {
                auto prev = std::prev( it );
                prevStart = prev->getStart();
                prevEnd   = prev->getEnd();
            }
        }
    }
}
