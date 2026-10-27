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
#ifndef KICK_XML_MANAGER_H_INCLUDE
    #define KICK_XML_MANAGER_H_INCLUDE

namespace mt
{
    const char* const __manufacturer = "Musictribe";
    class KickXML
        : private ValueTreeAllEventListener
        , private juce::AsyncUpdater
    {
    public:
        KickXML( juce::Identifier ID, const juce::String& FileExtension, const juce::String& Version );
        ~KickXML();

        std::function<void()> onLoad = []() {};

        // Main API's...
        juce::ValueTree loadOrCreate( const juce::File& XMLFIle );
        void            save();
        void            close();

        void                   setAutoSave( bool AutoSave ) { autoSave = AutoSave; }
        void                   setAutoSaveTime( int AutoSaveTimeInMS ) { autoSaveTimeOutInMS = AutoSaveTimeInMS; }
        juce::ValueTree&       getState() { return vt; }
        const juce::String&    getDirectory() { return xmlFileDir.getFullPathName(); }
        juce::CriticalSection& getCriticalSection() { return lock; };

    private:
        void valueTreeChanged() override;
        void handleAsyncUpdate() override;

    private:
        const juce::Identifier id;
        const juce::String     extn;
        const juce::String     version;

        juce::CriticalSection lock;
        juce::File            xmlFileDir;
        juce::File            xmlFile;
        juce::ValueTree       vt;
        juce::UndoManager     undo;

        static bool autoSave;
        static int  autoSaveTimeOutInMS;
        int         prefAutoSaveTimeOutInMS = 1500;

        struct pvtTimer : public juce::Timer
        {
            KickXML& Parent;
            explicit pvtTimer( KickXML& parent )
                : Parent( parent ) {}
            ~pvtTimer() { stopTimer(); }

            void timerCallback() override { Parent.triggerAsyncUpdate(); }
        };
        pvtTimer timer{ *this };
    };

}

#endif
