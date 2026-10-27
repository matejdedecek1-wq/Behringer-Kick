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

#include "juce_events/juce_events.h"

namespace ipc
{
	class MTIPCTimer : protected juce::Timer
	{
	public:
		MTIPCTimer(int intervalInMilliSeconds, std::function<void()> onTimeout, bool singleShot = true)
		: interval(intervalInMilliSeconds), singleShot(singleShot), onTimeout(onTimeout), timerStartFunction(std::bind(&ipc::MTIPCTimer::start, this)) { }

		virtual void timerCallback() override
		{
			if (singleShot)
			{
				stopTimer();
			}
			else
			{
				startAsync();
			}
			if(onTimeout) onTimeout();
			
		}

		void start()
		{
			startTimer(interval);
		}

		void setInterval(int intervalInMilliSeconds)
		{
			interval = intervalInMilliSeconds;
		}

		int getInterval()
		{
			return interval;
		}

		bool isRunning()
		{
			return isTimerRunning();
		}

		void startAsync()
		{
			juce::MessageManager::callAsync(timerStartFunction);
		}

		void stop()
		{
			if(isTimerRunning())
				stopTimer();
		}

	private:
		int interval;
		bool singleShot;
		std::function<void()> onTimeout;
		std::function<void()> timerStartFunction;
	};
}
