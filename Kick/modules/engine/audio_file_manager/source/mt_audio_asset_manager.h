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

namespace mt
{
    enum AssetStatus
    {
        AssetAlreadyExists,
        AssetAdded,
        AssetRemoved,
        AssetInvalid
    };

    class MTAudioAssetManager
    {
    public:
        std::function<void()> onDirChanged;

        explicit MTAudioAssetManager();
        ~MTAudioAssetManager() = default;

        void close();
        void setup( juce::ValueTree VT, juce::UndoManager* UM, const juce::String& ProjDir );

        void restoreFromValueTree( juce::ValueTree VT );

        void setRootFolder( const juce::String& ProjDir );

        const juce::File getSamplesDir() { return projDir.getChildFile( "/Imported" ); };
        const juce::File getExportsDir() { return projDir.getChildFile( "/Exported" ); };

        const juce::File& getProjDir() { return projDir; };
        const juce::File& getCurDir() { return curDir; }
        const juce::File& getInternalDir() { return internalDir; }
        const juce::File& getSoundsDir() { return soundsDir; }
        const juce::File& getDemosDir() { return demosDir; }
        const juce::File& getTempDir() { return tempDir; }

        const juce::Array<juce::File>& getDirectories() { return dirList; };
        const juce::Array<juce::File>& getFiles() { return fileList; };
        int                            getSelectedDir() { return selectedDir; };
        void                           setCurrentDir( juce::File& currDir );

        int  add( const juce::String& File, AssetTypes Type = AssetTypes::Samples );
        void remove( const juce::String& File, AssetTypes Type = AssetTypes::Samples );

        void dirSelect( int Index );
        void dirEnter( int Index );
        void dirGoUp();

        const std::vector<juce::String>& getAssets( AssetTypes Type = AssetTypes::Samples );
        std::vector<juce::String>        getAssetNames( AssetTypes Type = AssetTypes::Samples );
        const juce::String&              getAsset( AssetTypes Type = AssetTypes::Samples, int Index = 0 );
        juce::String                     getAssetName( AssetTypes Type = AssetTypes::Samples, int Index = 0 );

        ASound::Ptr getAssetSound( const juce::String& File );
        ASound::Ptr getAssetSound( int Index, AssetTypes Type = AssetTypes::Samples );

        std::vector<std::unique_ptr<ASoundSlice>>& getAssetSlices( const juce::String& File );
        std::vector<std::unique_ptr<ASoundSlice>>& getAssetSlices( int Index, AssetTypes Type = AssetTypes::Samples );

        ASoundSlice* getAssetSlice( const juce::String& File, int Index = 0 );
        void         addAssetSlice( const juce::String& File, ASoundSlice* Slice, int Index = -1 );
        void         removeAssetSlice( const juce::String& File, int Index );
        void         removeAllSlices( const juce::String& File );

        juce::ValueTree getAssetsVT() { return state; };

    private:
        ASoundCache&            asc{ getAudioSoundCache() };
        juce::ValueTree         state{ IDs::FILEMANAGER };
        juce::File              projDir, curDir, selDir, tempDir, soundsDir, demosDir, internalDir;
        juce::Array<juce::File> dirList{};
        juce::Array<juce::File> fileList{};
        juce::CriticalSection   lock;
        juce::UndoManager*      um{ nullptr };
        int                     selectedDir{ -1 };

        enum operation
        {
            ADD_ASSET,
            REMOVE_ASSET
        };

    private:
        std::vector<juce::String> samples;
        std::vector<juce::String> exports;
        std::vector<juce::String> audio;
        std::vector<juce::String> empty;

        std::map<juce::String, std::vector<std::unique_ptr<ASoundSlice>>> sliceMap;

        void updateDirectories( int Index );
        void updateFiles();

        void createDefaultSlice( juce::ValueTree fileVT, const juce::String& File );
        void restoreAsset( juce::ValueTree assetVT, AssetTypes Type );
        void restoreSlice( juce::ValueTree sliceVT, const juce::String& File );
        void addToAssetMgr( const juce::String& File, AssetTypes Type = AssetTypes::Samples );
        void removeFromAssetMgr( const juce::String& File, AssetTypes Type = AssetTypes::Samples );
        int  addRemoveInternal( operation AddOrRemove, const juce::String& File, AssetTypes Type );
    };
} // namespace mt
