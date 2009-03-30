// This file is part of Ell library.
//
// Ell library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Ell library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ell library.  If not, see <http://www.gnu.org/licenses/>.


D2(MOD_PROTECT, 1)                 // CTRL + MAJ + P
D2(MOD_UPPER_CASE, 2)              // CTRL + MAJ + U
D2(MOD_LOWER_CASE, 4)              // CTRL + U
D2(MOD_ERASE, 8)                   // CTRL + L
D2(MOD_UPPER_UNDERSCORE, 16)       // CTRL + _
D2(MOD_UPPER_LOWER, 32)            // CTRL + MAJ + _
D2(MOD_WORD, 64)                   // CTRL + W
D2(MOD_LINE, 128)                  // CTRL + MAJ + W
D2(MOD_COPY, 256)                  // CTRL + C
D2(MOD_CUT, 512)                   // CTRL + X
D2(MOD_BUFFER, 1024)               // CTRL + B
D2(MOD_REPEAT, 2048)               // CTRL + R
D2(MOD_SEARCH_FORWARD, 4096)       // CTRL + F
D2(MOD_SEARCH_BACKWARD, 8192)      // CTRL + MAJ + F

D(VALIDATE)                             // ENTER
D(CANCEL)                               // ECHAP
D(PASTE)                                // CTRL + V
D(UNDO)                                 // CTRL + Z
D(REDO)                                 // CTRL + Y
D(QUIT)                                 // CTRL + Q

D(MOVE_FORWARD)                         // LEFT
D(MOVE_BACKWARD)                        // RIGHT
D(MOVE_BEGIN_LINE)                      // HOME
D(MOVE_END_LINE)                        // END
D(MOVE_FORWARD_WORD)                    // CTRL + LEFT
D(MOVE_BACKWARD_WORD)                   // CTRL + RIGHT
D(MOVE_FORWARD_BLOC)                    // CTRL + SHIFT + RIGHT
D(MOVE_BACKWARD_BLOC)                   // CTRL + SHIFT + LEFT
D(MOVE_MATCHING_TAG)                    // CTRL + T

D(ERASE_BEGIN_LINE)                     // CTRL + SHIFT + BACKSPACE
D(ERASE_END_LINE)                       // CTRL + SHIFT + SUPPR
D(ERASE_BEGIN_WORD)                     // CTRL + BACKSPACE
D(ERASE_END_WORD)                       // CTRL + SUPPR
D(ERASE_WHOLE_LINE_OR_QUIT)             // CTRL + D
D(ERASE_FORWARD)                        // SUPPR
D(ERASE_BACKWARD)                       // BACKSPACE

D(WORD_COMPLETION_FORWARD)              // CTRL + N
D(WORD_COMPLETION_BACKWARD)             // CTRL + P
D(WORD_COMPLETION_COMMON_OR_LIST)       // TAB

D(HISTORY_MOVE_FORWARD)                 // DOWN
D(HISTORY_MOVE_BACKWARD)                // UP
D(HISTORY_COMPLETION_FORWARD)           // CTRL + UP
D(HISTORY_COMPLETION_BACKWARD)          // CTRL + DOWN
D(HISTORY_COMPLETION_COMMON_OR_LIST)    // CTRL + TAB

D(MACRO_BEGIN_END)                      // CTRL + SHIFT + M
D(MACRO_PLAY)                           // CTRL + M

D(INCREMENT_WORD)                       // CTRL + I
D(DECREMENT_WORD)                       // CTRL + MAJ + I

