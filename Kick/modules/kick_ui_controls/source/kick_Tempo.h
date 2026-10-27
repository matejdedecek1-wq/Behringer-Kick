/*
 * Copyright (c) 2024 MUSIC Tribe
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
    class KickTempoEditor
        : public KickControlBase<juce::Component>
        , public mt::MTProperties
        , private juce::ComponentListener
    {
    public:
        ~KickTempoEditor() = default;
        KickTempoEditor();

        static KickTempoEditor& Instance()
        {
            static KickTempoEditor editor;
            return editor;
        }

        void setRange( int Min, int Max, int Default, int Step, int Value );

        int  getValue() const noexcept { return static_cast<int>( slider.getValue() ); }
        void setValue( int Val );

        void show( const juce::String& Title, std::function<void( int )> CallbackOnExit );

    private:
        double getIncrement() override { return slider.getInterval(); }
        void   sliderMoved( float v ) override { slider.sliderMoved( v ); };
        void   knobRotated( double v ) override { slider.knobRotated( v ); };
        void   knobClicked() override { mt::utils::exitDialog( this, UI::APPLY ); };

    private:
        void reset();
        void clear();
        void addInput( int Value );
        void refresh();

        void resized();
        void paint( juce::Graphics& g ) override;
        void componentVisibilityChanged( Component& /*component*/ ) override
        {
            if( isShowing() )
                grabKeyboardFocus();
        }

    private:
        int def{ 0 };
        int interVal{ 0 };
        int maxValue{ 0 };
        int minValue{ 0 };
        // Tap Tempo related
        int    numTapsForTempo{ 3 };
        int    tapCount{ 0 };
        double startMs{ 0 };

        int                                                intermidentValue{ 0 };
        std::chrono::time_point<std::chrono::steady_clock> lastInputTime;
        std::chrono::milliseconds                          timeoutDuration{ 1000 }; // 2 seconds

        juce::TextEditor     inputTextLabel;
        juce::Rectangle<int> textBounds;

        KickImageButton num0{ "0", "Virtual_0", "", "Virtual_0_Active" };
        KickImageButton num1{ "1", "Virtual_1", "", "Virtual_1_Active" };
        KickImageButton num2{ "2", "Virtual_2", "", "Virtual_2_Active" };
        KickImageButton num3{ "3", "Virtual_3", "", "Virtual_3_Active" };
        KickImageButton num4{ "4", "Virtual_4", "", "Virtual_4_Active" };
        KickImageButton num5{ "5", "Virtual_5", "", "Virtual_5_Active" };
        KickImageButton num6{ "6", "Virtual_6", "", "Virtual_6_Active" };
        KickImageButton num7{ "7", "Virtual_7", "", "Virtual_7_Active" };
        KickImageButton num8{ "8", "Virtual_8", "", "Virtual_8_Active" };
        KickImageButton num9{ "9", "Virtual_9", "", "Virtual_9_Active" };
        KickImageButton del{ "Del", "numPad_D", "", "numPad_D_Active" };
        KickImageButton clr{ "Clear", "numPad_C", "", "numPad_C_Active" };
        KickImageButton inc{ "+", "numPad_P", "", "numPad_P_Active" };
        KickImageButton dec{ "-", "numPad_M", "", "numPad_M_Active" };

        std::array<KickToggleButton, 6> tapStype;
        std::array<KickButton, 5>       tapValue;

        KickSlider       slider;
        KickToggleButton tap{ "TAP TEMPO", "TAP TEMPO", "TAP TEMPO" };
    };

    class KickTempo
        : public KickControlBase<juce::Component>
        , public mt::MTProperties
        , private juce::AsyncUpdater
    {
    public:
        explicit KickTempo( const juce::String& Name, const int Min, const int Max, const int Default, const int StepSize = 1, bool ShowTitle = true, bool ShowBorder = false );

        ///
        /// Callbacks and Listener interface...
        ///
        std::function<void( int )> onValueChanged;

        class Listener
        {
        public:
            virtual ~Listener()                     = default; // Destructor.
            virtual void tempoChanged( KickTempo* ) = 0;
        };
        // Adds a listener to be called when this component's value changes.
        void addListener( KickTempo::Listener* l ) { listeners.add( l ); }
        // Removes a previously-registered listener.
        void removeListener( KickTempo::Listener* l ) { listeners.remove( l ); }

        ///
        /// Class methods...
        ///
        int getMin() const noexcept { return min; }
        int getMax() const noexcept { return max; }
        int getStep() const noexcept { return step; }
        int getDefault() const noexcept { return def; }
        int getValue() const noexcept { return value; }

        void setRange( int Min, int Max, int StepSize );
        void setMin( int Min );
        void setMax( int Max );
        void setStep( int Step ) { step = Step; }
        void setValue( int Value, juce::NotificationType Notify = juce::sendNotificationAsync );

        void  setNormalized( float Value ) { setValue( (int)norm.convertFrom0to1( (double)Value ), juce::sendNotification ); }
        float getNormalized() { return (float)norm.convertTo0to1( (double)value ); }
        void  knobRotated( double deltaValue ) override { setValue( (int)( value + deltaValue ) ); }
        void  handleAsyncUpdate() override;

    protected:
        double getIncrement() override { return (double)step; }
        void   knobClicked() override { showEditor(); }
        void   sliderMoved( float percentValue ) override;
        void   sendPercentValueToHardware( int value );

    private:
        void showEditor();
        void paint( juce::Graphics& g ) override;
        void mouseDoubleClick( const juce::MouseEvent& /*e*/ ) override { showEditor(); }

    private:
        bool isEditing{ false };
        int  min{ (int)BPM::MIN };
        int  max{ (int)BPM::MAX };
        int  def{ (int)BPM::DEFAULT };
        int  step{ 1 };
        int  value{ 0 };

        juce::NormalisableRange<double>         norm;
        juce::ListenerList<KickTempo::Listener> listeners;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickTempo )
    };
}
