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
#include <queue>
#include <string>
#include <fstream>

extern bool logging_allowed;

dmg07::dmg07(std::vector<gb*> g_gb) {

	v_gb.insert(v_gb.begin(), std::begin(g_gb), std::end(g_gb));

	for (int i = 0; i < v_gb.size(); i++)
	{
		cpu* cpu = v_gb[i]->get_cpu();
		cpu->m_dmg07 = this;
	}

}

void dmg07::reset() 
{
	mem.current_state = PING_PHASE;
	mem.transfer_count = 0;
	mem.phase_byte_count = 0;

	mem.restart_in = 0;
	mem.enter_status = 0x00;

	mem.packet_size = 0;
	mem.transfer_rate = 0x00;
	mem.transfer_speed = 512 * 8;
	mem.seri_occer = 2048 * 2048 * 2048;

	mem.first_aa_trans_nr = 0;
	mem.sync_trans_nr = 0;

	mem.delay = 0;
	//buffer_start_point = 0;

	//process_counter = 0;
	//process_occer = 4;

	mem.ready_to_sync_master = false;
	mem.master_is_synced = false;
	//ready_to_sync_others = false;
	//others_are_synced = false;

	mem.bytes_to_send = std::queue<byte>();

	for (byte i = 0; i < dmg07::v_gb.size(); i++)
	{
		mem.in_data_buffer[i] = 0;
		mem.trans_buffer[i].clear();
		mem.ans_buffer[i].clear();
		//last_trans_nr[i] = 0;
		//last_trans_nr[i] = 0;
	}
}

void dmg07::log_traffic(byte id, byte b)
{
	if (logging_allowed)
	{
		std::string filePath = "./dmg07_log.txt";
		std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
		if (id < v_gb.size())
		{
			ofs << "" << std::hex << (int)b << " ";

		}
		else
		{
			ofs << "" << std::hex << (int)b << std::endl;
		}

		ofs.close();
	}

}

void dmg07::log_save_state(char* data, size_t size)
{
	//if (logging_allowed)
	{
		{
			std::string filePath = "./dmg07_savesate_log.bin";
			std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);

			for (int i = 0; i < size; i++)
			{
				ofs << data[i];
			}
			ofs.close();
		}
	}
}


void dmg07::restart_pingphase()
{
	mem.current_state = PING_PHASE;
	mem.transfer_count = 0;

	mem.enter_status = 0x00;
	mem.packet_size = 0;
	mem.transfer_rate = 0x00;
	//transfer_speed = 70220 / 2;
	//seri_occer = 2042 * 16;
	mem.phase_byte_count = 0;
	mem.delay = 0;
	//ready_to_sync_others = false; 

	mem.bytes_to_send = std::queue<byte>();

	for (byte i = 0; i < dmg07::v_gb.size(); i++)
	{
		mem.trans_buffer[i].clear();
		mem.ans_buffer[i].clear();
		//last_trans_nr[i] = 0;
	}
}

void dmg07::handle_answer(int i, byte dat)
{
	switch (mem.current_state)
	{
	case PING_PHASE:
	{
	
		switch (mem.ans_buffer[i].size())
		{
		case 0:
		{
			if (dat == 0x88)
			{
				mem.ans_buffer[i].emplace_back(dat);
				//last_trans_nr[i] = transfer_count;
			}

			if (dat == 0xAA && i == 0) //&& ((transfer_count % 4) == 0) )
			{
				mem.ans_buffer[i].emplace_back(dat);
				mem.first_aa_trans_nr = mem.transfer_count % 4;
				mem.current_state = SYNC_PHASE;

				break;

			}
			break;
		}
		case 1:
		{
			if (dat == mem.ans_buffer[i].at(0)) //&& //last_trans_nr[i] == (transfer_count - 1))
			{
				mem.ans_buffer[i].emplace_back(dat);
				//last_trans_nr[i] = transfer_count;

				if (dat == 0x88)
				{
					mem.enter_status |= (byte)((byte)1 << (byte)(i + 4));
				}

			}
			else if (dat == 0xAA && i == 0)//&& ((transfer_count % 4) == 0))
			{
				mem.ans_buffer[i].clear();
				mem.ans_buffer[i].emplace_back(dat);
				mem.first_aa_trans_nr = mem.transfer_count % 4;
				mem.current_state = SYNC_PHASE;

			}
			else mem.ans_buffer[i].clear();


			break;
		}
		case 2:
		{

			if (dat == 0xAA && mem.ans_buffer[i].at(0) == 0xAA && mem.ans_buffer[i].at(1) == 0xAA && i == 0) //&& //last_trans_nr[i] == (transfer_count - 1))
			{
				mem.current_state = SYNC_PHASE;
				mem.ans_buffer[i].emplace_back(dat);
				break;


			}
			else if (mem.ans_buffer[i].at(0) == 0x88 && mem.ans_buffer[i].at(1) == 0x88)
			{
				mem.ans_buffer[i].emplace_back(dat);
				//last_trans_nr[i] = transfer_count;
				
				if (!mem.transfer_rate)
				{
					mem.transfer_rate = dat;
					
					/* Hack to get my assumed ping speedrate*/
					if (dat == 0xF0)  // Janshiro Games
						mem.transfer_speed = 5928 * 3;
					else if (dat == 0xA0)	// Jinsei Game Densetsz
						mem.transfer_speed = 5928 * 4;
					else if(dat == 0x85) // Top Rank Tennis
						mem.transfer_speed = 512 * 8;
					else {
						char ping_speed_multiplayer = dat & 0x0F;
						if (ping_speed_multiplayer)
						{
							if (ping_speed_multiplayer == 1 || ping_speed_multiplayer == 0)
								mem.transfer_speed = 512 * 16;
							else
								mem.transfer_speed = 512 * ping_speed_multiplayer;
						}
					}
						
				
					
					/*
					byte ping_rate = (byte)(transfer_rate & 0x0F);
					if(ping_rate)	transfer_speed = 2048 * (ping_rate); // PING SPEED
					*/
					
				}
			}
			else if (dat == 0xAA && i == 0)//&& ((transfer_count % 4) == 0))
			{
				mem.ans_buffer[i].clear();
				mem.ans_buffer[i].emplace_back(dat);
				mem.first_aa_trans_nr = mem.transfer_count % 4;
				mem.current_state = SYNC_PHASE;

			}
			else mem.ans_buffer[i].clear();

			break;
		}
		case 3:
		{
			if ((dat == 0xAA || dat == 0x00) && mem.ans_buffer[i].at(0) == 0xAA && mem.ans_buffer[i].at(1) == 0xAA && mem.ans_buffer[i].at(2) == 0xAA && i == 0) //&& //last_trans_nr[i] == (transfer_count - 1))
			{

				mem.current_state = SYNC_PHASE;
				mem.ans_buffer[i].clear();


			}
			else if (mem.ans_buffer[i].at(0) == 0x88 && mem.ans_buffer[i].at(1) == 0x88)
			{
				if (!mem.packet_size)
				{
					mem.packet_size = dat;
					
				}
			}
			else if (dat == 0xAA && i == 0)
			{
				mem.ans_buffer[i].clear();
				mem.ans_buffer[i].emplace_back(dat);
				mem.first_aa_trans_nr = mem.transfer_count % 4;
				mem.current_state = SYNC_PHASE;

			}

			mem.ans_buffer[i].clear();

			break;
		}
		}
		break;
	}
	case SYNC_PHASE:
		/*
		if (i == 0 && dat != 0xAA)
		{
			if (transfer_count % 4 == 0) return;
			//master_is_synced = true; 
			//sync_trans_nr = transfer_count;
			return;
		}
		if (i != 0 && dat == 0x00)
		{
			//if (transfer_count % 4 == 0) return;
			//others_are_synced = true;
			return;
		}
		*/
		break;
	case TRANSMISSION_PHASE:
	{
		if (i == 0)
		{
			switch (mem.ans_buffer[i].size())
			{
			case 0:
			{
				if (dat == 0xFF && mem.transfer_count % 4 == 0)
				{
					mem.ans_buffer[i].emplace_back(dat);
					//last_trans_nr[i] = transfer_count;
				}
				break;
			}
			case 1:
			{
				if (dat == 0xFF)
				{

					mem.ans_buffer[i].clear();
					mem.bytes_to_send = std::queue<byte>();
					mem.restart_in = mem.packet_size * 4;

					for (int i = 0; i < (mem.packet_size * 4); i++)
					{
						mem.bytes_to_send.push(0xFF);
					}
					mem.restart_in += mem.transfer_count;
					break;
				}
				else
				{
					mem.ans_buffer[i].clear();
					break;
				}
				break;
			}

			}

		}
		break;
	}
	}
}

void dmg07::send_byte(byte which, byte dat)
{
	byte ret = v_gb[which]->get_cpu()->seri_send(dat);

	if (which == 0) log_traffic(0, dat);
	log_traffic(which + 1, ret);

	handle_answer(which, ret);
}

void dmg07::broadcast_byte(byte dat)
{
	for (byte i = 0x00; i < (byte)dmg07::v_gb.size(); i++)
	{
		send_byte(i, dat);
	}
}

void dmg07::send_sync_bytes()
{
	
	if (!mem.ready_to_sync_master)
	{

		send_each_ping_byte();
		
		if (mem.transfer_count % 4 == 3)
			mem.ready_to_sync_master = true;

		return;
	}
	
	if (!mem.master_is_synced)
	{	
		if (mem.transfer_rate != 0xA0 && mem.transfer_rate != 0x85)	// Jinsei Game Densetsi and Top Rank Tennis
			mem.transfer_speed = 70216;

		broadcast_byte(0xCC);
		mem.master_is_synced = mem.transfer_count % 4 == 3;
		return;
	}

	if (++mem.phase_byte_count >= (mem.packet_size * 4) && mem.transfer_count % 4 == 0)
	{
		broadcast_byte(0x00);


		/*
		* This is just a hack, to match my assumend speed rate.
		* i didn't want to hardcode for every game. 
		* haven't found out the meaning of the magic-byte "transfer_rate" yet
		
		00 = 5928
		10 = 5928 * 2
		11 = 5928 * 4
		*/
	
		if (mem.transfer_rate == 0xF0)  //Janshiro Games
			mem.transfer_speed = 5928 * 3;
		else if (mem.transfer_rate == 0xA0)	// Jinsei Game Densetsi
			mem.transfer_speed = 5928 * 4;
		else {
			byte speed_multiplier = ((mem.transfer_rate & 0x80) >> 7) | ((mem.transfer_rate & 0x40) >> 5);
			mem.transfer_speed = 5928 * ((int)speed_multiplier + 1);
		}
	
		/*
		v_gb[0]->get_regs()->SC = transfer_rate;
		v_gb[0]->get_cpu()->seri_occer = seri_occer = 0x7fffffff;
		v_gb[0]->set_target(v_gb[1]);
		*/

		mem.current_state = TRANSMISSION_PHASE;
		mem.phase_byte_count = 0;
		mem.delay = 0;
		clear_all_buffers();
		return;
	}

}

void dmg07::send_ping_byte(byte which)
{
	byte dat = (mem.transfer_count % 4) ? (mem.enter_status | (byte)(which + 1)) : 0xFE;
	send_byte(which, dat);
}

void dmg07::send_each_ping_byte()
{
	for (byte i = 0x00; i < (byte)dmg07::v_gb.size(); i++)
		send_ping_byte(i);
}

void dmg07::clear_all_buffers() {
	for (byte i = 0; i < dmg07::v_gb.size(); i++) mem.trans_buffer[i].clear();
}

void dmg07::get_all_SC_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SC;
		mem.in_data_buffer[i] = data;
	}
};

void dmg07::get_all_SB_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SB;
		mem.in_data_buffer[i] = data;
	}
};

bool dmg07::is_expected_data(byte data)
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		if (mem.in_data_buffer[i] != data) return false;
	}
	return true;
}

bool dmg07::is_ready_to_process() {

	return (v_gb[0]->get_cpu()->total_clock > mem.seri_occer);
}

bool dmg07::all_IE_are_handled()
{

	for (int i = 0; i < v_gb.size(); i++)
	{
		if ((v_gb[i]->get_regs()->IF & v_gb[i]->get_regs()->IE & INT_SERIAL)) return false;
	}
	return true;

}

void dmg07::fill_buffer_for_less_than_4p() 
{
	if (v_gb.size() < 4)
	{
		for (int i = 0; i < 4 - v_gb.size(); i++)
		{
			for (int i = 0; i < mem.packet_size; i++)
			{
				mem.bytes_to_send.push(0);
			}
			
		}
	}
}

void dmg07::process()
{

	if (!is_ready_to_process()) return;

	switch (mem.current_state)
	{
		case PING_PHASE: send_each_ping_byte(); break;
		case SYNC_PHASE: send_sync_bytes(); break;
		case TRANSMISSION_PHASE:
		{

			// start buffering data
			if (mem.bytes_to_send.empty())	
			{
				//Get Packets
				for (byte i = 0; i < v_gb.size(); i++)
				{
					byte ret = v_gb[i]->get_cpu()->seri_send(0x00);
					//handle_answer(i, ret);

					if (i == 0)log_traffic(0, 0x00);
					log_traffic(i + 1, ret);

					if (mem.trans_buffer[i].size() < mem.packet_size)
					{
						mem.trans_buffer[i].emplace_back(ret);
	
					}

				}
				// if ready fill queue bytes_to_send
				//if (++delay == ((packet_size * 4) ))
				if (++mem.delay == ((mem.packet_size * 4) - 1))
				{
					for (byte i = 0; i < v_gb.size(); i++)
					{
						for (const auto& e : mem.trans_buffer[i])
							mem.bytes_to_send.push(e);

						mem.trans_buffer[i].clear();
					}
					mem.delay = 0;
					// buffer with zeros for less than 4 players
					fill_buffer_for_less_than_4p();
				}
				
				

			}

			//if queue ready send from queue and buffer new data
			else
			{
					//send packets and get new packets

					byte next_byte = mem.bytes_to_send.front();
					mem.bytes_to_send.pop();

					for (byte i = 0; i < v_gb.size(); i++)
					{
						byte ret = v_gb[i]->get_cpu()->seri_send(next_byte);

						if (i == 0)log_traffic(0, next_byte);
						log_traffic(i + 1, ret);

						if (mem.trans_buffer[i].size() < mem.packet_size && mem.delay > 0)
						{
							mem.trans_buffer[i].emplace_back(ret);
						}
					}
					mem.delay++;

					//refill queue
					if (mem.bytes_to_send.empty())
					{
						if (mem.transfer_count == mem.restart_in)
						{
							restart_pingphase();
							return;
						}

						for (byte i = 0; i < v_gb.size(); i++)
						{
							
							for (const auto& e : mem.trans_buffer[i])
								mem.bytes_to_send.push(e); // create bytes_to_send queue

							mem.trans_buffer[i].clear();
							
						}
						mem.delay = 0;
						fill_buffer_for_less_than_4p();
					}


				}

				break;
			}
	}

	mem.transfer_count++;
	mem.seri_occer = v_gb[0]->get_cpu()->total_clock + mem.transfer_speed;
}
	

void dmg07::save_mem_state(void* buf) {

	std::stringstream ss; // any stream can be used
	cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
	oarchive(this->mem); // Write the data to the archive
	
	dmg07_mem_state_size mem_size{};
	mem_size.size = ss.str().length();
	oarchive(mem_size);

	serializer s(buf, serializer::SAVE_BUF);
	s.process((void*)ss.str().data(), ss.str().length());
	buf += ss.str().length();
}


size_t dmg07::get_state_size() {

	std::stringstream ss; // any stream can be used
	cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
	oarchive(this->mem); // Write the data to the archive
	return sizeof(ss.str().data());
}


void dmg07::restore_mem_state(void* buf)
{
	
	
	std::stringstream ss2;
	ss2.write((const char*)buf, 248);

	cereal::BinaryInputArchive iarchive(ss2); // Create an input archive
	iarchive(this->mem);

	dmg07_mem_state_size mem_size{};
	iarchive(mem_size);

	buf += mem_size.size + sizeof(size_t); 

	/*
	serializer s(buf, serializer::LOAD_BUF);
	s.process(buf, sizeof(ss.str().data()));
	*/
	

}


