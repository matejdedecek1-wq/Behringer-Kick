namespace ui
{
    void KickStackedComponent::setCurentComponent( int index )
    {
        if( !stack.size() || index == currentIndex )
            return;

        if( juce::isPositiveAndBelow( index, (int)stack.size() ) )
        {
            for( auto it : getChildren() )
                it->setVisible( false );
            removeAllChildren();

            if( auto comp = stack[index] )
            {
                currentIndex = index;
                addChildComponent( comp );
                comp->setVisible( true );
            }
        }
    }

    juce::Component* KickStackedComponent::getComponent( int index )
    {
        return ( juce::isPositiveAndBelow( index, (int)stack.size() ) ) ? stack[index] : nullptr;
    }

    void KickStackedComponent::resized()
    {
        auto bounds = getLocalBounds();
        for( auto it : stack )
            it->setBounds( bounds );
    }

    void KickStackedComponent::addStackedComponent( juce::Component* component )
    {
        stack.add( component );
        component->setVisible( false );
    }

    void KickStackedComponent::removeStackedComponent( juce::Component* delComp, int indexToSwitch )
    {
        auto searchComp = [&]( juce::Component* Comp ) -> int {
            for( int idx = 0; idx < stack.size(); ++idx )
            {
                if( stack[idx] == Comp )
                    return idx;
            }
            return -1;
        };

        auto idx = searchComp( delComp );
        stack.remove( idx );
        setCurentComponent( indexToSwitch );
    }

    void KickStackedComponent::deleteAllStackedComponents()
    {
        for( int index = 0; index < stack.size(); index++ )
            delete stack[index];

        stack.clear();
    }
}
