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

#include <deque>
#include <memory>

namespace ipc
{
    template<typename T>
    class MTIPCMessageQueue
    {
    public:
        MTIPCMessageQueue() = default;
        ~MTIPCMessageQueue() { messageQueue.clear(); };

    public:
        void push( std::shared_ptr<T> message ) { messageQueue.push_back( message ); };
        std::shared_ptr<T> pop()
        {
            if( messageQueue.empty())
                return nullptr;
            std::shared_ptr<T> message = messageQueue.front();
            messageQueue.pop_front();
            return message;
        };
        int  size() { return messageQueue.size(); };
        bool isEmpty() { return messageQueue.empty(); }

    private:
        std::deque<std::shared_ptr<T>> messageQueue;
    };
}
