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

namespace kick_ui
{
    IRowView::IRowView( const juce::String& ViewName, const juce::Colour& BackgroundColor )
        : ui::KickTableView( ViewName, provider, BackgroundColor )
    {
        setRowHeight( ui::layout::tabRowHeight );
        setRowSpacing( ui::layout::tabRowSpacing );
        setScrollBarThickness( ui::layout::scrollBarThickness );
        setMargin( ui::layout::tabMargin );

        setRowBackgroundColour( ui::Theme::backgroundColour( 1 ) );
        setSelectedRowBackgroundColour( ui::Theme::backgroundColour( 0 ) );
        setRowOutlineColour( ui::Theme::outlineColour( 3 ) );
        setSelectedRowOutlineColour( ui::Theme::outlineColour( 1 ) );
        setAutoScrollToSelection( true );
    }

}
