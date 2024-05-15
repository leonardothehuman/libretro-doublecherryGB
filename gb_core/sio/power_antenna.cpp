/*--------------------------------------------------

   DoubleCherryGB - Gameboy Emulator (based on TGBDual)
   Copyright (C) 2023  Tim Oelrichs

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//-------------------------------------------------

#include "power_antenna.hpp"

byte power_antenna::seri_send(byte data) 
{
	if (data == 0x00) {
		power_antenna_on = false;
	}
	else if (!power_antenna_on)
	{
		power_antenna_on = true;
		
	}

	return power_antenna_on ? 0xF3 : 0xF2;
}