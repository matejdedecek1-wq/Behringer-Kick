#pragma once
namespace ui
{
    class KickToggleOptions
        : public KickControlBase<juce::Component>
        , public mt::MTProperties
    {
    public:
        std::function<void( const int Index, const juce::String& Option )> onSelectionChanged;

        explicit KickToggleOptions( const juce::String& Title, const std::vector<juce::String>& Options = {} );
        ~KickToggleOptions() = default;

        void setOptions( const std::vector<juce::String>& newOptions );

        void setSelected( const int Selection, juce::NotificationType notification );
        int  getSelected() const { return selected; }
        int  getSelectedOption() const { return selected; }

        void  setNormalized( float Value ) { setSelected( (int)( Value * options.size() ), juce::sendNotification ); }
        float getNormalized() { return (float)selected / (float)options.size(); }

        class Listener
        {
        public:
            virtual ~Listener()                                            = default; // Destructor.
            virtual void toggleOptionsChanged( KickToggleOptions* Number ) = 0; // For Application to implement
        };

        /** Adds a listener to be called when this component's value changes. */
        void addListener( KickToggleOptions::Listener* l ) { listeners.add( l ); }
        /** Removes a previously-registered listener. */
        void removeListener( KickToggleOptions::Listener* l ) { listeners.remove( l ); }

        void knobRotated( double deltaValue ) override;

    private:
        void paint( juce::Graphics& g ) override;
        void mouseDown( const juce::MouseEvent& event );

        void sliderMoved( float percentValue ) override;
        void knobClicked() override;
        void sendPercentValueToHardware();

    private:
        std::vector<juce::String>                       options;
        int                                             selected{ 0 };
        juce::NormalisableRange<int>                    norm;
        juce::ListenerList<KickToggleOptions::Listener> listeners;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickToggleOptions )
    };
}
