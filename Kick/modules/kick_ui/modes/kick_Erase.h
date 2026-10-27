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
class KickErase : public juce::Component
{
public:
    //==============================================================================
    KickErase();
    ~KickErase();

    void paint( juce::Graphics& g ) override;
    void resized() override;

    const mt::MTBarNBeats getStartBnb(void) {return startBnB; }
    const mt::MTBarNBeats getEndBnb(void) {return endBnB; }

    const mt::MTSelected getAeSelected(void) {return eraseSelect;}

private:
    ui::KickListBox        sequences{ui::KickListBox::withTitleBar,  "Sequence", {}  };
    ui::KickListBox        tracks{ui::KickListBox::withTitleBar, "Track", {}};

    ui::KickNumber         startBar{ "Bar", MIDI::MIN, MIDI::MAX, 0 };
    ui::KickNumber         endBar{ "Bar", MIDI::MIN, MIDI::MAX, 0 };
    ui::KickNumber         startBeat{ "Beat", MIDI::MIN, MIDI::MAX, 0 };
    ui::KickNumber         endBeat{ "Beat", MIDI::MIN, MIDI::MAX, 0 };

    int selectedSequenceIndex = 0;
    int selectedTrackIndex = 0;

    mt::MTSelected eraseSelect = {0};
    mt::MTBarNBeats startBnB = {0};
    mt::MTBarNBeats endBnB = {0};

        //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickErase )
};
