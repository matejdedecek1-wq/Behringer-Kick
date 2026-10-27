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
namespace kick_ui
{
    namespace StepPresetConstants
    {
        constexpr int enableButtonHeight = 34;
        constexpr int buttonHeight       = 40;

        constexpr int margin = 10;
    };

    class StepPresets;
    enum class StepPresetType
    {
        Ramp,
        Pulse,
        Triangle,
        Flip,
        Mirror,
        Inverse,
    };

    class IStepPreset : public juce::Component
        , private juce::AsyncUpdater
    {
    public:
        enum PresetValueType
        {
            Single = 1,
            Double,
            Quadruple,
            ReverseSingle,
            ReverseDouble,
            ReverseQuadruple,
            TypeMax,
        };

        explicit IStepPreset( StepPresetType Type, StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max );
        virtual ~IStepPreset() = default;

        std::function<void()> onClick;

        inline StepPresetType getType() const { return type; };

        bool isForward();

        virtual void calculateValues() = 0;
        void         sendClick( bool Active = false );

    private:
        void mouseUp( const juce::MouseEvent& ) override;
        void handleAsyncUpdate();
        void nextPresetValueType();
        void performCalculation();

    protected:
        const StepPresetType type;
        StepSequencerData&   data;
        std::vector<int>&    values;
        int&                 min;
        int&                 max;

        bool            isActive{ false };
        PresetValueType valueType   = PresetValueType::Single;
        unsigned int    presetCount = 1;
    };

    class StepRampPresets : public IStepPreset
    {
    public:
        explicit StepRampPresets( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max );

    private:
        void paint( juce::Graphics& g ) override;
        void calculateValues() override;

        std::vector<std::unique_ptr<juce::Drawable>> rampImages;
    };

    class StepPulsePresets : public IStepPreset
    {
    public:
        explicit StepPulsePresets( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max );

    private:
        void paint( juce::Graphics& g ) override;
        void calculateValues() override;

        std::vector<std::unique_ptr<juce::Drawable>> pulseImages;
    };

    class StepTrianglePresets : public IStepPreset
    {
    public:
        explicit StepTrianglePresets( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max );

    private:
        void paint( juce::Graphics& g ) override;
        void calculateValues() override;

        std::vector<std::unique_ptr<juce::Drawable>> triangleImages;
    };

    class StepPresetFlip : public IStepPreset
    {
    public:
        explicit StepPresetFlip( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max );

    private:
        void resized() override { flipPreset.setBounds( getLocalBounds() ); }
        void calculateValues() override;

        ui::KickImageButton flipPreset{ "FLIP", "Flip_Normal", "Flip_active" };
    };

    class StepPresetMirror : public IStepPreset
    {
    public:
        explicit StepPresetMirror( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max );

    private:
        void resized() override { mirrorPreset.setBounds( getLocalBounds() ); }
        void calculateValues() override;

        ui::KickImageButton mirrorPreset{ "MIRROR", "Mirror_Normal", "Mirror_active" };
    };

    class StepPresetInverse : public IStepPreset
    {
    public:
        explicit StepPresetInverse( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max );

    private:
        void resized() override { inversePreset.setBounds( getLocalBounds() ); }
        void calculateValues() override;

        ui::KickImageButton inversePreset{ "INVERSE", "Inverse_Normal", "Inverse_active" };
    };

    class StepPresets : public juce::Component
    {
    public:
        StepPresets( StepSequencerData& Data );

        std::function<void( std::vector<int>& )> addValues;
        std::function<void( std::vector<int>& )> modifyValues;

        std::function<void( int Value )> sliderOffsetOnDragOn;
        std::function<void( int Value )> sliderOffsetOnDragEnd;

        void              setActivePreset( StepPresetType type );
        std::vector<int>& getPresetValues() { return presetValues; }
        int               setPresetValues( std::vector<int>& Values, int Min, int Max );

        inline int getOffset() { return offset; }

    private:
        void paint( juce::Graphics& g );
        void resized() override;
        void drawMarks( juce::Graphics& g );

        inline bool isPresetEnabled() { return enablePreset.getToggleState(); }

    private:
        StepSequencerData&   data;
        std::vector<int>     presetValues;
        ui::KickToggleButton enablePreset{ "Presets", "Presets", "Presets" };

        ui::KickSlider levelSlider{ "", juce::Slider::LinearBarVertical, juce::Slider::NoTextBox };

        int              min{ 0 }, max{ 127 };
        int              offset        = 0;
        int              previousValue = 0;
        std::list<float> markings;

        StepRampPresets     ramp{ data, presetValues, min, max };
        StepTrianglePresets triangle{ data, presetValues, min, max };
        StepPulsePresets    pulse{ data, presetValues, min, max };
        StepPresetFlip      flip{ data, presetValues, min, max };
        StepPresetMirror    mirror{ data, presetValues, min, max };
        StepPresetInverse   inverse{ data, presetValues, min, max };
    };
}
