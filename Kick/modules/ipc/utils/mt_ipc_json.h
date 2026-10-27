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
#include <memory>

namespace juce
{
	class String;
	class var;
}
namespace ipc
{
	class MTIPCJsonImpl;
	class MTIPCJson
	{
	public:
		MTIPCJson();
		MTIPCJson(const juce::String& absfilePath);
		MTIPCJson(const MTIPCJson& copy);
		MTIPCJson& operator=(const MTIPCJson& copy);
		virtual ~MTIPCJson();

	public:
		void             parseFile(const juce::String& absFilePath);
		void             parseString(const juce::String& jsonStr);
		void             setProperty(const juce::String& dotSeparatedKey, const juce::var& value);
		const juce::var& getProperty(const juce::String& dotSeparatedKey);
		const juce::var& getProperty(const juce::String& dotSeparatedKey, const juce::var& defultValue);
		const juce::var& getRoot();
		void             copy(const MTIPCJson& rhs);

	private:
		std::unique_ptr<MTIPCJsonImpl> _impl_;
	};

}
