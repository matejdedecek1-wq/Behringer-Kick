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

class AppHumanize : public juce::Component,
    public ui::KickButton::Listener,
    public ui::KickSlider::Listener
{
public:

    AppHumanize();
    ~AppHumanize() override;

    void paint( juce::Graphics& g ) override;
    void resized() override;
    void buttonClicked( juce::Button* buttonThatWasClicked ) override;
    void sliderValueChanged( juce::Slider* sliderThatWasMoved ) override;
    void updateTrackInfo( AppTrackComponent::trackData_t* data );
    const Controls::track::humanizeInfo& getHumanizeData( void ) const { return(humanizeData); }
private:

    Label timeLabel;
    Label noteInputFilter;
    ui::KickToggleButton lengthSelect;
    ui::KickButton humanizeApply;
    ui::KickButton humanizeCancel;
    ui::KickToggleButton humanizeTimeSelection;
    ui::KickSlider splitSlider;
    Label intoLabel;
    Label timeSliderNotify;
    ui::KickSlider timeSlider;
    Label timeAmountLabel;
    Label eagernessNotify;
    ui::KickSlider eagernessSlider;
    Label eagernessLabel;
    Label lengthLabel;
    ui::KickSlider lengthSlider;
    Label lengthValueLabel;
    ui::KickToggleButton velocitySelect;
    Label velocityLabel;
    ui::KickSlider velocitySlider;
    Label strengthLabel;
    ui::KickToggleButton onlySelectedCheck;
    Label inputFilterLabel;
    std::unique_ptr<AppTrackComponent::trackData_t> trackDataInfoPtr;
    Controls::track::humanizeInfo humanizeData;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppHumanize )
};

//[EndFile] You can add extra defines here...
//[/EndFile]
