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

namespace mt{
    class MTSampleEditorActionsInterface;
    /**
    * MTSampleEditUndoAction is the class used to wrap actions derived from MTSampleEditorActionsInterface and perform undo and redo.
    * before performing a action MTSampleEditUndoAction copies the original region being edited to a undo file and then performs action.
    * MTSampleEditorFromDestFileAction is the action used to copy the original content from undo file to the main file in undo function.
    * MTSampleEditorFromDestFileAction can handle restoring of samples modified by processBetweenRegion, processOutofRegion process types.
    * if in future MTSampleEditorFromDestFileAction cannot handle all the undo functionality of different actions. other undo actions can be
    * implemented to suit the new action requirments.
    **/
    class MTSampleEditUndoAction : public juce::UndoableAction{
    public:
        explicit MTSampleEditUndoAction( MTSampleEditorActionsInterface* action );
        ~MTSampleEditUndoAction();
        bool perform() override;
        bool undo() override;
        static void setClearHistory( bool setClear );
        int getSizeInUnits() override;
        static void setUndoActions( std::list<MTSampleEditUndoAction*>* actions ){ undoActions = actions; }
        void reduceSize( juce::int64 noSamples );
        static void cleanup();

    private:
        void backupOriginalSample();
        void removeOriginalSampleFromBackup();
        juce::int64 getTotalSizeInSamples();

    private:
        std::unique_ptr<MTSampleEditorActionsInterface> actionImp = nullptr;
        static juce::File undoFile;
        // start and of sample in temp file whee the original undo segment is stored. in case of out of region processing start and end of first and last segment
        juce::int64 startOfSample = -1;
        juce::int64 endOfSample = -1;
        juce::int64 startOfSample1 = -1;
        juce::int64 endOfSample1 = -1;
        static bool clearHistory;
        static int bitDeapth;
        static int numChannels;
        static std::list<MTSampleEditUndoAction*>* undoActions;
    };
    /**
    * MTSampleEditorActionsInterface is the interface all the sample editor actions like normalize, gain change have to implement. the main functions of the interface are
    *
    * virtual void initialize(const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source) which initializes parameters, region, and source. the derived call can overrid this function to initialize
    * the type of processing or perform any other calculations.
    * virtual bool execute();.  the process of editing the region of a sample  between start and endsample involve copying the samples from 0 to start to a tempfile. and call performaction on samples
    * between start and endSample. the execute function starts this process of copying samples to temp file and replacing the temp file with original file.
    * virtual bool performAction(juce::AudioSampleBuffer& buffer, int& numSamples) = 0; is the function every derived class has to implement this function returns true if there are more samples to be processed
    * the idea is to process the samples between start and end (in case of processBetweenRegion process type) in smaller buffer size rather than whole samples between start and end.
            processBetweenRegion,
            processOutofRegion,
            noProcessing
        are the processing types suported.
     in case of processBetweenRegion, performAaction is called on samples between start and end sample.
     in case of processOutofRegion, performAaction is called on samples between o to start and end to total length
     in case of noProcessing, execute calls perform Action and returns.

     all the actions can be executed independently as shown below
        MTSampleEditorDeleteAction* actionObject = new MTSampleEditorDeleteAction;
        actionObject->initialize(region, params, sourceFile);
        actionObject->execute();

           or

        wrap the action in MTSampleEditUndoAction and perform undo redo on it as shown below.
        getAudioInterface().getUndoManager().perform(new MTSampleEditUndoAction(actionObject));

    **/

    class MTSampleEditorActionsInterface{
    public:
        enum ProcessType    {
            processBetweenRegion,
            processOutofRegion,
            noProcessing
        };
        MTSampleEditorActionsInterface();

        virtual ~MTSampleEditorActionsInterface(){}
        virtual void initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source );
        virtual bool canUndo(){ return true; }
        virtual bool execute();

        virtual bool hasState(){ return true; }

        // getter setters
        const MTSampleRegion& getRegion(){ return region; }
        juce::AudioFormatManager& getFormatManager(){ return formatManager; }
        juce::File& getSourceFile(){ return sourceFile; }
        MTSampleEditorActionsInterface::ProcessType getProcessType(){ return type; }
        void setProcessType( MTSampleEditorActionsInterface::ProcessType newType ){ type = newType; }
        juce::int64 getTotalSamples(){ return totalSmaples; }

    protected:
        int read( juce::AudioSampleBuffer& buffer, juce::int64 startSample, juce::int64 endSample, juce::AudioFormatReader* r, juce::int64 current );
        void executeprocessOutofRegion();
        void executeprocessBetweenRegion();
        void cleanupReaderWriter( juce::String& tempPath );
        void initializeReaderWriter( juce::String& tempPath );
        juce::String checkForDuplicateAndConstructNewPath( const juce::String path );
        bool extractFromSample( juce::int64 startSample, juce::int64 endSample, const juce::String& destFileName );

        // to perform action from start to end saple block by block returns true if more blocks are to be processed
        virtual bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) = 0;

    protected:
        MTSampleRegion region;
        juce::AudioFormatManager formatManager;
        juce::File sourceFile;
        std::list<juce::var> params;
        juce::AudioFormatReader* reader = nullptr;
        juce::AudioFormatWriter* writer = nullptr;
        juce::int64 startSample = 0;
        juce::int64 endSample = 0;
        juce::int64 currentSample = 0;
        int regionProcessing = -1;
        juce::int64 totalSmaples = -1;
        ProcessType type = processBetweenRegion;
    };

    ///////////////////////////////////////////
    /// ///////// reverse action
    //////////////////////////////////////////
    class MTSampleEditorReverseAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorReverseAction() = default;
        bool hasState() override{ return false; }

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;

    private:
        juce::int64 reverseCurrentSample = -1;
    };

    ///////////////////////////////////////////
    /// ///////// normalise
    //////////////////////////////////////////
    class MTSampleEditorNormaliseAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorNormaliseAction() = default;
        virtual void initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source );

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;

    private:
        float gainFactor = 0.0f;
    };

    ///////////////////////////////////////////
    /// ///////// fadein action
    //////////////////////////////////////////
    class MTSampleEditorFadeInAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorFadeInAction() = default;

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };

    ///////////////////////////////////////////
    /// ///////// fadeout action
    //////////////////////////////////////////
    class MTSampleEditorFadeOutAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorFadeOutAction() = default;

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorFromDestFileAction
    //////////////////////////////////////////
    class MTSampleEditorFromDestFileAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorFromDestFileAction( juce::int64 start, juce::int64 end, juce::int64 start1, juce::int64 end1, juce::int64 originalLength, juce::File& destFile );
        bool execute() override;

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
        void deleteRegionFromDest( juce::AudioFormatReader* destReader );

    private:
        juce::int64 destStartSample;
        juce::int64 destEndSample;
        juce::int64 destStartSample1;
        juce::int64 destEndSample1;
        juce::int64 originalLength;
        juce::File& destFile;
        juce::int64 currentDestSample;
        int regionsProcessed = -1;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorExtractAction
    //////////////////////////////////////////
    class MTSampleEditorExtractAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorExtractAction() = default;
        void initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source );
        bool canUndo() override{ return false; }
    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorSilenceAction
    //////////////////////////////////////////
    class MTSampleEditorSilenceAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorSilenceAction() = default;

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorDiscardeAction
    //////////////////////////////////////////
    class MTSampleEditorDiscardAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorDiscardAction() = default;
        void initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source );

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorDeleteAction
    //////////////////////////////////////////
    class MTSampleEditorDeleteAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorDeleteAction() = default;

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorGainChangeAction
    //////////////////////////////////////////
    class MTSampleEditorGainChangeAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorGainChangeAction() = default;
        void initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source ) override;

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;

    private:
        float gain = 1;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorCopyAction
    //////////////////////////////////////////
    class MTSampleEditorCopyAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorCopyAction() = default;
        void initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source ) override;
        bool canUndo(){ return false; }

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorStereoToMonoAction
    //////////////////////////////////////////
    class MTSampleEditorStereoToMonoAction : public MTSampleEditorActionsInterface{
    public:
        MTSampleEditorStereoToMonoAction() = default;
        void initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source ) override;
        bool canUndo(){ return false; }

    private:
        bool performAction( juce::AudioSampleBuffer& buffer, int& numSamples ) override;
    };
}
