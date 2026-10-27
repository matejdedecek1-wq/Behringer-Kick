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

namespace mt
{
    KickXML::KickXML( juce::Identifier ID, const juce::String& FileExtension, const juce::String& Version )
        : id( ID ), extn( FileExtension ), version( Version )
    {
        vt.addListener( this );
    }

    KickXML::~KickXML()
    {
        save();
    }
    bool KickXML::autoSave{ true };
    int  KickXML::autoSaveTimeOutInMS{ 1500 };
    void KickXML::valueTreeChanged()
    {
        auto xml = vt.toXmlString();
        if( vt.getType() == IDs::PREFERENCES )
            timer.startTimer( prefAutoSaveTimeOutInMS );
        else
        {
            if( !autoSave )
                return;

            if( !timer.isTimerRunning() )
                timer.startTimer( autoSaveTimeOutInMS );
        }
    }

    void KickXML::handleAsyncUpdate()
    {
        timer.stopTimer();
        save();
    }

    juce::ValueTree KickXML::loadOrCreate( const juce::File& XMLFIle )
    {
        juce::ScopedLock l( lock );

        // Close if already opened...
        close();

        xmlFileDir    = XMLFIle.exists() ? XMLFIle.isDirectory() ? XMLFIle : XMLFIle.getParentDirectory() : XMLFIle;
        auto fileName = xmlFileDir.getFileName() + extn;
        xmlFile       = xmlFileDir.getChildFile( fileName );

        if( !xmlFile.exists() )
        {
            // Create.
            xmlFile.create();
        }
        if( xmlFile.getSize() == 0 )
        {
            // Create basic structure,
            vt = juce::ValueTree{ id };
            vt.setProperty( IDs::manufacturer, __manufacturer, nullptr );
            vt.setProperty( IDs::version, version, nullptr );
            save();
        }
        // Load.
        vt = juce::ValueTree::fromXml( xmlFile.loadFileAsString() );

        onLoad(); // Invoke callback.
        return vt;
    }

    void KickXML::save()
    {
        if( xmlFile.existsAsFile() )
        {
            juce::ScopedLock l( lock );
            // Save Program...
            auto xml  = vt.toXmlString();
            auto fxml = xmlFile.loadFileAsString();
            if( fxml.compare( xml ) )
                xmlFile.replaceWithText( xml );
        }
    }

    void KickXML::close()
    {
        save();
        juce::ScopedLock l( lock );

        xmlFile    = "";
        xmlFileDir = "";
    }

}
