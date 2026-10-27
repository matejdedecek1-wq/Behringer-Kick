
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

#include "juce_gui_basics/juce_gui_basics.h"
#include "../../config/ipc_config_inteface.h"
#include <memory>

namespace ipc
{
    class MTIIPCMessageHandler;
    class MTIPCMessageDispatcher;
    class MTIPCSubsystem;
}

namespace contracts
{
    class IPCApplicationImpl;
    class IPCApplication : public juce::JUCEApplication
    {
    public:
        IPCApplication( const juce::String& name,
                        const juce::String& logFileBaseName,
                        const juce::String& metricsFileBaseName,
                        bool                createSystemInterface = false,
                        bool                enableHeartBeat       = false,
                        bool                logToServer           = false,
                        const juce::String& version               = "0.0.1" );
        ~IPCApplication();
        const juce::String                           getApplicationName() override;
        const juce::String                           getApplicationVersion() override;
        void                                         initialise( const juce::String& commandLine ) override;
        void                                         shutdown() override;
        std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher();
        std::shared_ptr<ipc::MTIPCSubsystem>         subsystem();

    protected:
        virtual void addMessageHandlers( std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers ) = 0;
        virtual juce::var getConfig(const juce::String& cmdline) = 0;
    private:
        std::unique_ptr<contracts::IPCApplicationImpl> impl;
    };

}
