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
namespace ui
{
    class KickInsertEffects : public juce::Component
    {
    public:
        std::function<void( void )> onMouseDown;
        KickInsertEffects();
        ~KickInsertEffects();

        void                       setInsertList( std::vector<juce::String>& List );
        std::vector<juce::String>& getInsertList() { return inserts; };

    private:
        void paint( juce::Graphics& g ) override;
        void resized() override;
        void mouseDown( const juce::MouseEvent& ) override;

    private:
        KickLabel                    title{ "INSERTS" };
        std::array<ui::KickLabel, 4> insertLabels;
        std::vector<juce::String>    inserts;
    };

    class KickIOComponent : public juce::Component
    {
    public:
        KickIOComponent();

        SIGNALS : sigslot::signal<int, const juce::String&> inputChanged;
        sigslot::signal<int, const juce::String&>           outputChanged;
        sigslot::signal<float>                              gainChanged;
        sigslot::signal<double>                             panSliderChanged;
        sigslot::signal<bool>                               recordButtonStateChanged;
        // Monitor Level.
        sigslot::signal<bool>       monitorStateChanged;
        sigslot::signal<double>     monitorLevelChanged;
        std::function<void( void )> onInsertsClicked;

        void showPanSlider( bool Show )
        {
            bShowPan = Show;
            resized();
        }
        void showRecordButton( bool Show )
        {
            bShowRec = Show;
            resized();
        }

        void showOutputs( bool Show )
        {
            bShowOuts = Show;
            resized();
        }

        int  getNumChannels();
        void signalStrengthChanged( const std::vector<float>& strength );
        void setSignalStrength( float lrms, float rrms, float lpeak, float rpeak );

        // Pan Slider...
        void   setPan( double Pan ) { panB.setValue( Pan, juce::dontSendNotification ); };
        double getPan() const { return panB.getValue(); };

        // Gain Slider...
        void   setGain( double Value ) { gainMeter.setValue( Value, juce::dontSendNotification ); }
        double getGain() { return gainMeter.getValue(); }

        // Inputs...
        inline int getInputSelection() { return inputList.getSelectedIndex(); }
        inline int getOutputSelection() { return outputList.getSelectedIndex(); }

        inline void setInputSelection( int Index, juce::NotificationType Notify ) { inputList.setSelected( Index, Notify ); }
        inline void setOutputSelection( int Index, juce::NotificationType Notify ) { outputList.setSelected( Index, Notify ); }

        // functions to populate values
        void setRangeAndDisplayValues( const juce::Point<float>& range, const std::vector<float>& displayValues );
        void setInputs( const std::vector<juce::String>& input, int Selected );
        void setOutputs( const std::vector<juce::String>& output, int Selected );
        void setOptions1( const std::vector<juce::Component*>& options );
        void reset();

        // Monitor
        void setMonitor( bool state, juce::NotificationType Notify = juce::dontSendNotification ) { monitor.setToggleState( state, Notify ); }
        bool isMonitorOn() const noexcept { return monitor.getToggleState(); }

        void   setMonitorLevel( double Level, juce::NotificationType Notify = juce::dontSendNotification ) { monitorSlider.setValue( Level, Notify ); }
        double getMonitorLevel() const noexcept { return monitorSlider.getValue(); }

        void   setMonitorLevelNormalized( double Level, juce::NotificationType Notify = juce::dontSendNotification ) { monitorSlider.setNormalized( Level, Notify ); }
        double getMonitorLevelNormalized() const noexcept { return monitorSlider.getNormalized(); }

        void knobCommand( int knob, int value );
        void knobRotated( double deltaValue );

        void setInsertsList( std::vector<juce::String>& InsertsList );

    private:
        void visibilityChanged() override;
        void resized() override;

    private:
        bool            bShowPan{ true };
        bool            bShowRec{ true };
        bool            bShowOuts{ true };
        KickListBox     inputList{ KickListBox::BoxType::withoutTitleBar, "", {} };
        KickListBox     outputList{ KickListBox::BoxType::withoutTitleBar, "", {} };
        KickPanRotary   panB;
        KickGainSlider  gainMeter;
        KickLevelMeter  levelMeter;
        KickImageButton recordB{ "record", "Looper_rec", "Looper_rec_active" };
        KickImageButton monitor{ "Monitor", "sampler_Monitor", "sampler_Monitor_active" };
        KickSlider      monitorSlider{ "Monitor Level", juce::Slider::LinearBar, juce::Slider::TextBoxBelow };

        KickInsertEffects             ieff;
        std::vector<juce::Component*> op1;
    };
}
