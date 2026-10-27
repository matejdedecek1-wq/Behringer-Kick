#include "kick_SampleEditorSettings.h"
namespace mt
{
    namespace ac
    {
        SampleEditorSettings::SampleEditorSettings( SampleEditorData& data )
            : seData( data )
        {
            utils::addAndMakeVisible( this, cuePlayLabel, slicePreview1Label, slicePreviewLabel, cuePreviewLabel, autoScrollLabel, timeLineUnitsLabel, toggleOneShot, cuePreviewList, slicePreviewList, slicePreview1List, autoScrollList, timeLineUnitsList );
            utils::setValueColour( ui::Theme::textColour( 1 ), cuePlayLabel, slicePreview1Label, slicePreviewLabel, cuePreviewLabel, autoScrollLabel, timeLineUnitsLabel );
            utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), cuePlayLabel, slicePreview1Label, slicePreviewLabel, cuePreviewLabel, autoScrollLabel, timeLineUnitsLabel );
            utils::setValueFontHeight( 16, cuePlayLabel, slicePreview1Label, slicePreviewLabel, cuePreviewLabel, autoScrollLabel, timeLineUnitsLabel );
            toggleOneShot.setValueFontHeight( 18 );

            toggleOneShot.onClick = [&]() {
            };

            cuePreviewList.onSelectionChanged = [&]( int Index, const juce::String& ) {
                cuePreview = Index == 0 ? SampleEditorData::CuePreview::cuePreviewOff : Index == 1 ? SampleEditorData::CuePreview::cuePreviewBefore :
                                                                                                     SampleEditorData::CuePreview::cuePreviewAfter;
            };

            slicePreviewList.onSelectionChanged = [&]( int Index, const juce::String& ) {
                sPreview = Index == 0 ? SampleEditorData::SlicePreview::slicePreviewOff : Index == 1 ? SampleEditorData::SlicePreview::slicePreviewBefore :
                                                                                                       SampleEditorData::SlicePreview::slicePreviewAfter;
            };

            slicePreview1List.onSelectionChanged = [&]( int Index, const juce::String& ) {};
            autoScrollList.onSelectionChanged    = [&]( int Index, const juce::String& ) {
                aScroll = Index == 0 ? SampleEditorData::AutoScroll::autoScrollOff : Index == 1 ? SampleEditorData::AutoScroll::autoScrollFollow :
                                                                                                     SampleEditorData::AutoScroll::autoScrollPage;
            };

            timeLineUnitsList.onSelectionChanged = [&]( int Index, const juce::String& ) {
                tUnits = Index == 0 ? SampleEditorData::TimeLineUnits::unitsTime : Index == 1 ? SampleEditorData::TimeLineUnits::unitsSamples :
                                                                                                SampleEditorData::TimeLineUnits::unitsBeats;
            };

            cuePreviewList.setSelected( seData.getCuePreview() == SampleEditorData::CuePreview::cuePreviewOff ? 0 : seData.getCuePreview() == SampleEditorData::CuePreview::cuePreviewBefore ? 1 :
                                                                                                                                                                                               2 );
            slicePreviewList.setSelected( seData.getSlicePreview() == SampleEditorData::SlicePreview::slicePreviewOff ? 0 : seData.getSlicePreview() == SampleEditorData::SlicePreview::slicePreviewBefore ? 1 :
                                                                                                                                                                                                             2 );
            autoScrollList.setSelected( seData.getAutoScroll() == SampleEditorData::AutoScroll::autoScrollOff ? 0 : seData.getAutoScroll() == SampleEditorData::AutoScroll::autoScrollFollow ? 1 :
                                                                                                                                                                                               2 );
            timeLineUnitsList.setSelected( seData.getTimeLineUnits() == SampleEditorData::TimeLineUnits::unitsTime ? 0 : seData.getTimeLineUnits() == SampleEditorData::TimeLineUnits::unitsTime ? 1 :
                                                                                                                                                                                                   2 );
        }

        void SampleEditorSettings::resized()
        {
            cuePlayLabel.setBoundsRelative( .1892f, .0737f, .1846f, .1081f );
            cuePreviewLabel.setBoundsRelative( .1892f, .2334f, .1846f, .1081f );
            slicePreviewLabel.setBoundsRelative( .1892f, .3931f, .1846f, .1081f );
            slicePreview1Label.setBoundsRelative( .1892f, .5528f, .1846f, .1081f );
            autoScrollLabel.setBoundsRelative( .1892f, .7125f, .1846f, .1081f );
            timeLineUnitsLabel.setBoundsRelative( .1892f, .8722f, .1846f, .1081f );

            toggleOneShot.setBoundsRelative( .6276f, .0737f, .1846f, .1081f );

            cuePreviewList.setBoundsRelative( .3892f, .2334f, .423f, .1081f );
            slicePreviewList.setBoundsRelative( .3892f, .3931f, .423f, .1081f );
            slicePreview1List.setBoundsRelative( .3892f, .5528f, .423f, .1081f );
            autoScrollList.setBoundsRelative( .3892f, .7125f, .423f, .1081f );
            timeLineUnitsList.setBoundsRelative( .3892f, .8722f, .423f, .1081f );
        }

        bool SampleEditorSettings::hasTimeLineUnitsChanged()
        {
            return ( tUnits != seData.getTimeLineUnits() );
        }

        bool SampleEditorSettings::hasAutoScrollChanged()
        {
            return ( aScroll != seData.getAutoScroll() );
        }

        bool SampleEditorSettings::hasCuePreviewChanged()
        {
            return ( cuePreview != seData.getCuePreview() );
        }

        bool SampleEditorSettings::hasSlicePreciewChanged()
        {
            return ( sPreview != seData.getSlicePreview() );
        }

    }
}
