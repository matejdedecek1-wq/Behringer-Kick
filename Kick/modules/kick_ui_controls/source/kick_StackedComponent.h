#pragma once
namespace ui
{
    class KickStackedComponent : public juce::Component
    {
    public:
        void addStackedComponent( juce::Component* component );
        void removeStackedComponent( juce::Component* component, int indexToSwitchTo = 0 );
        void deleteAllStackedComponents();

        void             setCurentComponent( int index );
        juce::Component* getComponent( int index );

        inline int       getCurrentIndex() const { return currentIndex; }
        inline int       getSize() const { return (int)stack.size(); }
        juce::Component* getCurentComponent() { return stack[currentIndex]; };

        void  setNormalized( float Value ) { setCurentComponent( (int)( Value * stack.size() ) ); }
        float getNormalized() { return (float)currentIndex / (float)stack.size(); }

    private:
        juce::Array<juce::Component::SafePointer<juce::Component>> stack;
        int                                                        currentIndex{ -1 };

    protected:
        void resized() override;
    };
}
