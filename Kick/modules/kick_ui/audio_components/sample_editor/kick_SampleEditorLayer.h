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

#include "kick_SampleEditorLayerInterface.h"

#pragma once
namespace mt
{
    namespace ac
    {
        enum SampleEditorLayerType
        {
            layerType1,
            layerType2,
            layerType3,
            layerType4
        };

        class SampleEditorLayerOptionsInterface : public juce::Component
        {
        public:
            virtual ~SampleEditorLayerOptionsInterface() {}
            virtual void setSeData( SampleEditorData* /*data*/ ) {}
            virtual void setSamplePath( const juce::String& /*sample*/ ) {}
            virtual void reload() {}
        };

        class SampleEditorLayerBase : public SampleEditorLayerInterface
        {
        public:
            SampleEditorLayerBase( SampleEditorLayerOptionsInterface* opt );
            ~SampleEditorLayerBase();
            void             resized() override;
            juce::Component* getSampleLayerSettingsComponent() override;
            void             loadSample( const juce::String& path ) override;

        private:
            ui::KickViewport                                   scaleViewport;
            ui::KickViewport                                   seViewport;
            SampleEditorData                                   seData;
            SEScaleComponent                                   scale{ seData };
            SEViewComponent                                    editor{ seData, &seViewport };
            SampleLayerSettings                                settings{ seData };
            std::unique_ptr<SampleEditorLayerOptionsInterface> layerOptions;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SampleEditorLayerBase )
        };

        class SampleEditorLayer : public SampleEditorLayerBase
        {
        public:
            SampleEditorLayer();
        };

        class SampleEditorLayerKG : public SampleEditorLayerBase
        {
        public:
            SampleEditorLayerKG();
        };

        class SampleEditorLayerClip : public SampleEditorLayerBase
        {
        public:
            SampleEditorLayerClip();
        };

    }
}
