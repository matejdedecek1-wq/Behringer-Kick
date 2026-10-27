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

namespace mt
{
    ///
    /// INSERTS
    ///

    struct MTInsertsParamInfo
    {
        juce::String name{ "" };
        juce::String unit{ "" };
        juce::String displayValue{ "" };
        float        value{ 0.f };
    };

    using InsertsParams = std::vector<MTInsertsParamInfo>;
    struct MTInsertsInfo
    {
        juce::String  name{ "" };
        bool          bypass{ false };
        InsertsParams params;
    };

    ///
    struct MTIInsertsImpl;
    class MTIInsertsInterface
    {
    public:
        MTIInsertsInterface();
        ~MTIInsertsInterface();
        void setup( juce::ValueTree& VT, juce::UndoManager* UM, AudIO::DawInserts* Impl );

        void                        getInserts( std::vector<juce::String>& InsertNames );
        std::vector<MTInsertsInfo>* getInserts();
        MTInsertsInfo               getInsertsInfo( unsigned int Position );
        std::vector<bool>           getInsertBypassState();

        bool isAllInsertsBypassed();
        bool isInsertBypassed( unsigned int Position );
        void setAllInsertsBypass( bool Bypass );
        int  setInsertBypass( unsigned int Position, bool Bypass );

        int  addInsert( const juce::String& Effect, unsigned int Position );
        int  removeInsert( const juce::String& Effect );
        int  removeInsert( unsigned int Position );
        void removeAllInserts();

        int          getNumInserts();
        int          getNumParameters( unsigned int Position );
        juce::String getInsertName( unsigned int InsertPosition );
        juce::String getParameterName( unsigned int InsertPosition, int Parameter );
        juce::String getParameterValueString( unsigned int InsertPosition, int Parameter );

        float getParameterValue( unsigned int InsertPosition, int Parameter );
        void  setParameterValue( unsigned int InsertPosition, int Parameter, float Value );

    private:
        std::unique_ptr<MTIInsertsImpl> pImpl;
        juce::ValueTree                 state;
        juce::UndoManager*              um{ nullptr };
        std::vector<juce::String>       inserts;
    };

} // namespace mt
