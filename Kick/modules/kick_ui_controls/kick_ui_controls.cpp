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

#ifdef KICK_UI_CONTROLS_INCLUDE
/* When you add this cpp file to your project, you mustn't include it in a file where you've
     already included any other headers - just put it inside a file on its own, possibly with your config
     flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
     header files that the compiler may be using.
  */
    #error "Incorrect use of JUCE cpp file"
#endif

// Module include files...
#include "kick_ui_controls.h"

// Module source files...
#include "source/kick_StackedComponent.cpp"
#include "source/kick_GainSlider.cpp"
#include "source/kick_MultiSelectTabBarButton.cpp"
#include "source/kick_Theme.cpp"
#include "source/kick_lookAndFeel.cpp"
#include "source/kick_utils.cpp"
#include "source/kick_utils_loadingAnimation.cpp"
#include "source/kick_buttons.cpp"
#include "source/kick_label.cpp"
#include "source/kick_number.cpp"
#include "source/kick_slider.cpp"
#include "source/kick_listbox.cpp"
#include "source/kick_levelMeter.cpp"
#include "source/kick_viewport.cpp"
#include "source/kick_separator.cpp"
#include "source/kick_table.cpp"
#include "source/kick_TimerControl.cpp"
#include "source/kick_ToggleOptions.cpp"
#include "source/kick_CyclicStateControl.cpp"
#include "source/kick_OffsetControl.cpp"
#include "source/kick_VirtualKeyboard.cpp"
#include "source/kick_ProgressBar.cpp"
#include "source/kick_AlertWindow.cpp"
#include "source/kick_AnimatedText.cpp"
#include "source/kick_image.cpp"
#include "source/kick_serialNumber.cpp"
#include "source/kick_Tempo.cpp"
#include "source/kick_Grid.cpp"
#include "source/kick_WaveRenderer.cpp"
#include "source/kick_NoteSelector.cpp"
#include "source/kick_IPv4Editor.cpp"
