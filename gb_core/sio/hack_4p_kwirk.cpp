/*--------------------------------------------------

   DoubleCherryGB - Gameboy Emulator - 4Players (based on TGBDual)
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

#include "gb.h"
#include <vector>


hack_4p_kwirk::hack_4p_kwirk(std::vector<gb*> g_gb)
{
	v_gb = g_gb;
	reset();
}


byte hack_4p_kwirk::seri_send(byte data)
{
	switch (current_state)
	{
	case TITLE_SCREEN:
	{

		bool all_are_ready = true,

			get_all_SB_reg_data();

		//check who is master, who send 0x66 
		for (int i = 0; i < v_gb.size(); i++)
		{
			if (in_data_buffer[i] == 0x66) master_id = i;
		}
		redirect_data(data);

		//check if all_are_ready
		for (int i = 0; i < v_gb.size(); i++)
		{
			if (i == master_id) continue;
			if (in_data_buffer[i] != 0x9F) all_are_ready = false;
		}

		if (all_are_ready)
		{
			return 0x9F;
		}

		return 0xFF;

		break;
	}
	case SELECT_SCREEN:
	{
		if (data == 0x08)
		{
			current_state = SELECT_SKILL;
			redirect_data(data);
			return 0xFC;
		}
		return 0xFF;
		break;
	}
	case SELECT_SKILL:
	{
		if (data == 0x08) current_state = SELECT_SKILL;

		redirect_data(data);
		return 0xEE;

		break;
	}
	case SELECT_COURSE_RMS:
	{

		break;
	}

	case SELECT_CONTEST:
		break;
	case INGAME:
		break;
	case WINNER_SCREEN:
		break;
	case NEXT_MATCH_SCREEN:
		break;
	default:
		break;
	}



}

void hack_4p_kwirk::get_all_SB_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SB;
		in_data_buffer[i] = data;
	}
}

void hack_4p_kwirk::redirect_data(byte data)
{

	for (int i = 0; i < v_gb.size(); i++)
	{
		if (i != master_id)
			in_data_buffer[i] = v_gb[i]->seri_send(data);
	}
}

byte hack_4p_kwirk::get_highest_value()
{
	get_all_SB_reg_data();
	byte max_value = 0;

	for (int i = 0; i < v; i++)
	{
		if (i != master_id);
		if (in_data_buffer[i] > max_value) max_value = in_data_buffer[i];
	}
}

void hack_4p_kwirk::reset()
{
	current_state = TITLE_SCREEN;
	byte data_buffer[] = { 0,0,0,0 };
}