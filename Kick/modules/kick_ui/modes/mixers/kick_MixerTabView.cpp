#include "kick_MixerTabView.h"
namespace ui
{
#define mixerTabWidth 355

    MixerTabComponent::MixerTabComponent()
    {
        mt::utils::addChildComponents( this, ioComp, padGroupComp );
    }

    void MixerTabComponent::setButtonType( int buttonRange, KickPadButton::PadButtonType type )
    {
        padGroupComp.setButtonType( buttonRange, type );
    }

    void MixerTabComponent::setButtonNames( const std::list<juce::String>& labels )
    {
        padGroupComp.setButtonNames( labels );
    }

    void MixerTabComponent::resized()
    {
        ioComp.setBounds( 0, 10, mixerTabWidth, getHeight() - 10 );
        padGroupComp.setBounds( ioComp.getRight() + 10, 10, getWidth() - mixerTabWidth - 10, getHeight() - 10 );
    }

    void MixerTabComponent::visibilityChanged()
    {
        bool visible = isVisible();
        for( auto it : getChildren() )
            it->setVisible( visible );
    }

    MixerTabView::MixerTabView( MixerHandlerInterface* hand )
        : KickTabbedComponent( juce::TabbedButtonBar::TabsAtTop )
    {
        setTabBarDepth( 44 );
        setIndent( 10 );

        onMultiSelctTabChanged = [this]( int tabIndex, int /*multiSelectIndex*/, const juce::String& /*NewTabName*/ ) {
            if( handler )
            {
                TabData tbData = handler->getTabData()[(unsigned int)tabIndex];
                // change the button type displayed in current tab
                //MixerTabComponent* tab = static_cast<MixerTabComponent*>(getTabContentComponent(tabIndex));
                //tab->setButtonType()}
            }
        };

        onTabChanged = [this]( int newCurrentTabIndex, const juce::String& /*newCurrentTabName*/ ) {
            if( handler && 0 <= newCurrentTabIndex )
            {
                const std::vector<TabData>& tabD = handler->getTabData();
                if( newCurrentTabIndex < (int)tabD.size() )
                {
                    tab.getPads().setButtonType( tabD[newCurrentTabIndex].multiTabButtonTypes[0] );
                }
                handler->onLoad();
            }
        };

        if( hand ) resetHandler( hand );
    }

    MixerTabView::~MixerTabView()
    {
    }

    void MixerTabView::resetHandler( MixerHandlerInterface* hand )
    {
        clearTabs();
        handler = hand;
        if( handler )
        {
            tab.getIO().setInputs( handler->getInputs(), 0 );
            tab.getIO().setOutputs( handler->getOutputs(), 0 );
            tab.getIO().setOptions1( handler->getIoOptions1() );
            //tab.getIO().setOptions2( handler->getIoOptions2() );
            tab.getIO().setRangeAndDisplayValues( handler->getLevelRange(), handler->getLevelDisplay() );

            const std::vector<TabData>& tabD     = handler->getTabData();
            int                         tabIndex = 0;
            for( const TabData& data : tabD )
            {
                if( 1 < data.multiTabCount )
                    setMultiselectTab( tabIndex, data.multiTabCount );

                addTab( TRANS( data.tabName ), ui::Theme::backgroundColour( 5 ), &tab, false );
                tabIndex++;
            }
            handler->onLoad();
        }
        else
        {
            addTab( TRANS( "Not Available" ), ui::Theme::backgroundColour( 5 ), &na, false );
        }
    }

    void MixerTabView::visibilityChanged()
    {
        bool visible = isVisible();
        for( auto it : getChildren() )
            it->setVisible( visible );
    }

}
