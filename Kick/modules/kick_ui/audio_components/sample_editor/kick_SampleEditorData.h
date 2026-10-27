/*
 * Copyright (c) 2021 MUSIC Tribe
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

#pragma once
namespace mt
{
    namespace ac
    {
        /*!
        * Regions class manages creation, deletion, modification of regions. all the changes done to a region are stored in
        * value tree. any changes done to the regions can be  undon and redone.
        */
        class SampleEditorData;

        enum LoopMode
        {
            loopModeNone,
            loopModeForward,
            loopModeBidirection,
            loopModeBackward
        };
#define loopPosPropertyId         "loopPos"
#define loopModeNonePId           "none"
#define loopModeForwardPId        "F"
#define loopModeBackdPId          "B"
#define loopModeBiDirdPId         "BD"

#define seSettingsTuPropertyName  "TimeLineUnits"
#define timelineIdBeats           "beats"
#define timelineIdSamples         "samples"
#define timelineIdTime            "time"

#define seSettingsCuePlay         "CuePlay"
#define cuePlayIdToggle           "toggtle"
#define cuePlayIdOneshot          "oneShot"

#define seSettingsCuePreview      "CuePreview"
#define cuePreviewIdOff           "off"
#define cuePreviewIdBefore        "before"
#define cuePreviewIdAfter         "after"

#define seSettingsSlicePreview    "SlicePreview"
#define slicePreviewIdOff         "off"
#define slicePreviewIdBefore      "before"
#define slicePreviewIdAfter       "after"

#define seSettingsAutoScroll      "AutoScroll"
#define autoScrollIdOff           "off"
#define autoScrollIdFollow        "follow"
#define autoScrollIdPage          "page"

#define selectedIndexPropertyName "selectedIndex"

        class Region
        {
        public:
            friend class Regions;
            Region( juce::ValueTree s )
                : state( s )
            {
                updateValues();
            }

            Region() {}

            static juce::ValueTree createRegion( juce::int64 start, juce::int64 end, juce::UndoManager* undoManager, bool all = false )
            {
                juce::ValueTree region = all ? juce::ValueTree( "REGION_ALL" ) : juce::ValueTree( "REGION" );
                region.setProperty( "start", start, undoManager );
                region.setProperty( "end", end, undoManager );
                region.setProperty( "loopMode", loopModeNonePId, undoManager );
                region.setProperty( loopPosPropertyId, start, undoManager );
                return region;
            }

            void setStart( juce::int64 value, juce::UndoManager* undoManager )
            {
                if( undoManager )
                {
                    state.setProperty( "start", value, undoManager );
                    double loopPosTmp = state.getProperty( loopPosPropertyId );
                    if( loopPosTmp < value )
                        setLoopPos( value, undoManager );
                }
                else
                {
                    if( loopPos < value )
                        setLoopPos( value, nullptr );
                }

                start = value;
            }

            void setEnd( juce::int64 value, juce::UndoManager* undoManager )
            {
                if( undoManager )
                {
                    state.setProperty( "end", value, undoManager );
                    double loopPosTmp = state.getProperty( loopPosPropertyId );
                    if( loopPosTmp > value )
                        setLoopPos( value, undoManager );
                }
                else
                {
                    if( loopPos > value )
                        setLoopPos( value, nullptr );
                }
                end = value;
            }

            bool isSelected() const { return selected; }

            bool operator==( const Region& lhs ) const
            {
                return lhs.start == start && lhs.end == end && lhs.selected == selected;
            }

            juce::ValueTree& getState() { return state; }

            juce::int64 getStart() const { return start; }

            juce::int64 getEnd() const { return end; }

            void updateValues()
            {
                start                     = state.getProperty( "start" );
                end                       = state.getProperty( "end" );
                juce::String propertyName = state.getProperty( "loopMode" );
                loopPos                   = state.getProperty( loopPosPropertyId );

                if( propertyName == loopModeNonePId )
                    loopMode = LoopMode::loopModeNone;
                else if( propertyName == loopModeForwardPId )
                    loopMode = LoopMode::loopModeForward;
                else if( propertyName == loopModeBiDirdPId )
                    loopMode = LoopMode::loopModeBidirection;
                else
                    loopMode = LoopMode::loopModeBackward;
            }

            bool isValid() const { return ( start >= 0 && end >= 0 ); }

            void setLoopMode( LoopMode newLoopMode, juce::UndoManager* undoManager )
            {
                if( !state.isValid() )
                    return;
                juce::String propertyName;
                if( newLoopMode == LoopMode::loopModeNone )
                    propertyName = loopModeNonePId;
                else if( newLoopMode == LoopMode::loopModeForward )
                    propertyName = loopModeForwardPId;
                else if( newLoopMode == LoopMode::loopModeBidirection )
                    propertyName = loopModeBiDirdPId;
                else
                    propertyName = loopModeBackdPId;

                state.setProperty( "loopMode", propertyName, undoManager );
                loopMode = newLoopMode;
            }

            LoopMode getLoopMode() const
            {
                return loopMode;
            }

            void setLoopPos( juce::int64 value, juce::UndoManager* undoManager )
            {
                if( undoManager )
                    state.setProperty( loopPosPropertyId, value, undoManager );
                else
                    loopPos = value;
            }

            juce::int64 getLoopPos() const
            {
                return loopPos;
            }

            int                 getIndex() const { return index; }
            const juce::String& getStartLabel() const { return startLabel; }
            const juce::String& getEndLable() const { return endLabel; }

        private:
            void setSelected( bool newSelectedState )
            {
                selected = newSelectedState;
            }

            void setIndex( int newIndex )
            {
                index = newIndex;
                if( index == -1 )
                {
                    setStartLabel( juce::String( "S" ) );
                    setEndLabel( juce::String( "E" ) );
                }
                else
                {
                    juce::String label( index );
                    setStartLabel( label );
                    setEndLabel( label );
                }
            }

            void setStartLabel( const juce::String& label ) { startLabel = label; }
            void setEndLabel( const juce::String& label ) { endLabel = label; }

        private:
            juce::ValueTree state;
            bool            selected = false;
            juce::int64     start    = -1;
            juce::int64     end      = -1;
            LoopMode        loopMode = LoopMode::loopModeNone;
            juce::int64     loopPos  = 0;
            juce::String    startLabel{ "S" };
            juce::String    endLabel{ "E" };
            int             index = 0;
        };

        class Regions : public std::list<Region>
            , public juce::ValueTree::Listener
            , public juce::ChangeListener
        {
        public:
            Regions( juce::ValueTree& s, juce::UndoManager& undoM )
                : state( s )
                , undoManager( undoM )
            {
                selectedIndex = state.getProperty( selectedIndexPropertyName );
                undoManager.addChangeListener( this );
            }
            ~Regions() { undoManager.removeChangeListener( this ); }
            void                        update();
            void                        setSedata( SampleEditorData* d );
            void                        valueTreePropertyChanged( juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property ) override;
            void                        valueTreeChildRemoved( juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved ) override;
            void                        valueTreeChildAdded( juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded ) override;
            void                        pop_back();
            std::list<Region>::iterator erase( std::list<Region>::iterator it );
            std::list<Region>::iterator insert( std::list<Region>::iterator it, Region& region );
            void                        createEqualRegions( unsigned int slices );
            const Region&               getRegion( int pos );
            const Region&               getSelectedRegion() const;
            void                        loopSelectedRegion();
            void                        setSelectedRegionLoopMode( LoopMode newMode );
            LoopMode                    getSelectedRegionLoopMode() const;
            void                        joinRegions();
            Region&                     splitAtTime( juce::int64 start, bool selectNewRegion );
            void                        selectRegion( const Region& regionToSelect );
            void                        selectRegion( const int regionIndex );
            void                        splitSelectedRegion();
            void                        deleteRegion( const Region& region );
            void                        clearAllRegions();
            // changes loop position of selected Region
            void        setLoopPos( juce::int64 pos, bool commit );
            juce::int64 getLoopPosition() const;
            void        ensureSelection();
            Region&     getAllRegion() { return allRegion; }
            void        changeListenerCallback( juce::ChangeBroadcaster* source );
            size_t      size();

        private:
            int  getSelectedIndex();
            void updateIndex();
            void findEnd( juce::int64& end, juce::int64 start, std::list<Region>::iterator& it );

        private:
            Region             allRegion;
            Region             invalidRegion;
            juce::ValueTree&   state;
            juce::UndoManager& undoManager;
            SampleEditorData*  seData                 = nullptr;
            bool               ignoreValueTreeChanges = false;
            int                selectedIndex          = 0;
            bool               selectedIndexDirty     = false;
        };

        /*!
        * SampleEditorData managers all the data related to samples like regions.
        * different components in sample editor use SampleEditorData to set and retrive values.
        * SampleEditorData notifies changes made by one componet to other components using different signals
        * like regionChanged, regionAdded...
        * SampleEditorData also handles all the communication with the backend
        */

        //TODO Segregate the interfce further to settings and transport and move signals to their respective classes
        // and move all the region related operations to regions maybe rename SampleEditorData to SampleEditorManager since it manages different classes
        class SampleEditorData : public MTSamplePlayerListener /*,
                                 public MTSampleModifiedListener*/
        {
        public:
            enum class Mode
            {
                modeNone,
                modeTrim,
                modeChop,
                modeProgram
            };
            enum SplitMode
            {
                splitModeNone,
                splitModeManual,
                splitModeRegions,
                splitModeBpm,
                splitModeThreshold
            };

            enum class TimeLineUnits
            {
                unitsNone,
                unitsTime,
                unitsSamples,
                unitsBeats
            };

            enum class CuePlay
            {
                cuePlayToggle,
                cuePlayOneShot
            };

            enum class CuePreview
            {
                cuePreviewOff,
                cuePreviewBefore,
                cuePreviewAfter
            };

            enum class SlicePreview
            {
                slicePreviewOff,
                slicePreviewBefore,
                slicePreviewAfter
            };

            enum class AutoScroll
            {
                autoScrollOff,
                autoScrollFollow,
                autoScrollPage
            };

            enum class SeTool
            {
                seToolEdit,
                seToolErase,
                seToolListen,
                seToolZoom
            };

            SampleEditorData();
            ~SampleEditorData();
            void                      setView( juce::Component* v ) { view = v; }
            void                      initialize( const juce::String& audioSamplePath );
            juce::AudioThumbnailBase* getThumbnail();
            double                    getTotalBeats();

            // region
            Region&       splitAtTime( juce::int64 start, bool selectNewRegion = true );
            Region&       getLastRegion();
            bool          moveTime( const Region& range, juce::int64 newTime, bool end, bool commit = false );
            void          selectRegion( const Region& region );
            void          selectRegion( int index );
            void          splitAtCuePos();
            void          splitSelectedRegion();
            void          joinSelectedAndPrev();
            size_t        getTotalRegions() { return regions.size(); }
            void          removeLastRegion();
            bool          isEnd( const Region& region );
            int           getPosition( const Region& region );
            const Region& getSelectedRegion();
            void          deleteRegion( const Region& region );
            bool          getLinkSlices() { return linkSlices; }
            void          setLinkSlices( bool link );
            void          setLoopPosition( juce::int64 pos, bool commit = false );
            juce::int64   getLoopPosition();
            void          clearAllRegions();

            juce::ValueTree    getSampleState() { return sampleState; }
            juce::UndoManager* getUndoManager() { return &undoManager; }

            double                           getSampleLength();
            juce::int64                      getSampleLengthInSamples();
            double                           detectBpm() { return bpm; }
            const std::vector<juce::String>& getSampleList();
            const juce::String&              getSamplePath() { return currentSamplePath; }
            juce::int64                      convertTimeToSamples( double time );
            double                           convertSamplesToTime( juce::int64 sample );
            void                             performAction( unsigned int actionId, const std::list<juce::var>& params );
            juce::int64                      getNextZeroSnapPos( juce::int64 pos, bool forward );
            void                             splitByBpm( std::vector<juce::var>& params );
            void                             splitByThresh( std::vector<juce::var>& params );

            void                   setMode( SampleEditorData::Mode mode );
            SampleEditorData::Mode getMode() { return editMode; }
            bool                   isTrimMode();
            bool                   isChopMode();
            void                   setLoopLock( bool lock )
            {
                loopLock = lock;
                loopLockChanged( loopLock );
            }
            bool                 getLoopLock() { return loopLock; }
            float                getPosInPixels( juce::int64 posTime );
            juce::int64          getPosInSeconds( float posInPix );
            juce::Rectangle<int> getViewArea() { return view->getLocalBounds(); }
            void                 setCuePosition( juce::int64 newPosition );
            juce::int64          getCuePosition();
            // called by value tree property change in undo
            void                        setSplitMode( SplitMode newSplitMode );
            void                        setSplitMode( SplitMode newSplitMode, std::vector<juce::var>& params );
            SampleEditorData::SplitMode getSpliMode();
            bool                        isListenTool();
            bool                        isSplitModeManual();
            std::vector<juce::String>&  getSupportedActions() { return supportedActions; }
            std::vector<unsigned int>   getDisabledAction();
            void                        setNoSlices( unsigned int slices );
            void                        setLoopMode( LoopMode newLoopMode );
            LoopMode                    getLoopMode() { return regions.getSelectedRegionLoopMode(); }
            void                        setTool( SeTool tool ) { selectedTool = tool; }
            SeTool                      getTool() { return selectedTool; }
            void                        enableZeroCrossing( bool enable ) { zeroCrossing = enable; }
            bool                        isZeroCrossingEnabled() { return zeroCrossing; }

            // settings
            void          setTimeLineUnits( TimeLineUnits units );
            TimeLineUnits getTimeLineUnits() { return tUnits; }
            AutoScroll    getAutoScroll() { return autoScroll; }
            void          setAutoScroll( AutoScroll newScroll );
            double        getNumberOfBeats();
            void          setCuePreview( CuePreview cPreview );
            CuePreview    getCuePreview() { return cuePreview; };
            void          setSlicePreview( SlicePreview sPreview );
            SlicePreview  getSlicePreview() { return slicePreview; }

            void undo();
            void redo();

            // transport
            void playSelectedRegion( MTSamplePlayMode mode );
            void play( int slice = -1 );
            void playLoop();
            void play( const Region& region, MTSamplePlayMode mode = MTSamplePlayMode::samplePlayFromStart );
            void play( const MTSampleRegion& region, MTSamplePlayMode mode = MTSamplePlayMode::samplePlayFromStart );
            void playCue();
            void stop();
            bool isRunning() { return isPlaying; }
            // transport callback
            void positionChanged( juce::int64 pos ) override;
            void stopped() override;
            void sampleModified() override;

            SIGNALS : sigslot::signal<const Region&> regionChanged;
            sigslot::signal<const Region&>           regionAdded;
            sigslot::signal<const Region&>           regionAboutToBeRemoved;
            sigslot::signal<const Region&>           regionRemoved;
            sigslot::signal<>                        sampleAboutToBeChanged;
            sigslot::signal<>                        sampleChanged;
            sigslot::signal<SampleEditorData::Mode>  modeChanged;
            sigslot::signal<bool>                    loopLockChanged;
            sigslot::signal<juce::int64>             cuePositionChanged;
            sigslot::signal<juce::int64>             loopPositionChanged;
            sigslot::signal<>                        settingsChanged;
            sigslot::signal<juce::int64>             playHeadChanged;
            sigslot::signal<>                        playbackStopped;
            sigslot::signal<bool>                    linkSlicesChanged;
            sigslot::signal<SplitMode>               splitModeChanged;
            sigslot::signal<>                        currentSampleModified;
            sigslot::signal<const Region&>           regionSelected;
            sigslot::signal<const Region&>           regionMoved;

        private:
            void linkSlicesImp();
            void splitByThreshImp( std::vector<juce::var>& params );
            void splitByBpmImp( std::vector<juce::var>& params );
            void createValueTreeItem();
            void findEnd( double& end, double start, std::list<Region>::iterator& it );
            void getNextStartAndEnd( std::list<Region>::iterator& it, juce::int64& prevStart, juce::int64& prevEnd );
            void getPrevStartAndEnd( std::list<Region>::iterator& it, juce::int64& prevStart, juce::int64& prevEnd );

        private:
            double                    totalBeats;
            MTSampleEditorInterface*  sei = nullptr;
            int                       bitsPerSample;
            int                       sampleRate;
            int                       totalSamples;
            Mode                      editMode   = Mode::modeTrim;
            bool                      linkSlices = false;
            double                    bpm;
            Region                    invalidRange;
            juce::Component*          view        = nullptr;
            bool                      loopLock    = false;
            juce::int64               cuePosition = 0;
            SplitMode                 splitMode   = SplitMode::splitModeManual;
            int                       noOfSlices  = 16;
            std::vector<juce::String> supportedActions;
            juce::String              currentSamplePath;
            bool                      zeroCrossing = false;
            juce::int64               playHeadPos  = -1;
            bool                      isPlaying{ false };

            juce::ValueTree    sampleEditorState;
            juce::ValueTree    sampleState;
            juce::ValueTree    sampleStateLayer1;
            juce::ValueTree    regionState;
            juce::UndoManager& undoManager;
            Regions            regions{ regionState, undoManager };

            //settings
            TimeLineUnits tUnits     = TimeLineUnits::unitsSamples;
            CuePlay       cuePlay    = CuePlay::cuePlayToggle;
            CuePreview    cuePreview = CuePreview::cuePreviewOff;
            AutoScroll    autoScroll = AutoScroll::autoScrollOff;
            SlicePreview  slicePreview;
            SeTool        selectedTool;
        };
    }
}
