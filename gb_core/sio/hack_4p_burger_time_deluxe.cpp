
#include "gb.h"
#include <array> 
#include <vector>
#include <queue>
#include <ctime>
#include <cstdlib>
#include <fstream>


hack_4p_burger_time_deluxe::hack_4p_burger_time_deluxe(std::vector<gb*> g_gb) {

	v_gb.insert(v_gb.begin(), std::begin(g_gb), std::end(g_gb));

	mem.game_state = TITLE_SCREEN;

	mem.seri_occer = 28885392;
	mem.transfer_speed = 70212;

	init_send_data_queue();
}

void hack_4p_burger_time_deluxe::init_send_data_queue() {

	char start_sequence[11] = { 0xf0, 0xf0, 0xf0, 0xf0, 0xf2, 0xf2, 0xf0, 0xf0,0xf0 ,0xf0, 0xf0};
	fill_send_data_queue(start_sequence); 
	
}

void hack_4p_burger_time_deluxe::fill_send_data_queue(char sequence[])
{
	int len = sizeof(sequence) / sizeof(char);

	for (int i = 0; i < len; ++i)
		mem.send_data_queue.push(sequence[i]);
}

void hack_4p_burger_time_deluxe::get_all_SC_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SC;
		mem.in_data_buffer[i] = data;
	}
};

void hack_4p_burger_time_deluxe::get_all_SB_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SB;
		mem.in_data_buffer[i] = data;
	}
};

bool hack_4p_burger_time_deluxe::is_expected_data(byte data)
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		if (mem.in_data_buffer[i] != data) return false;
	}
	return true;
}

bool hack_4p_burger_time_deluxe::is_ready_to_process() {

	get_all_SC_reg_data();
	return is_expected_data(0x80) && all_IE_are_handled();
}

bool hack_4p_burger_time_deluxe::all_IE_are_handled()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		if ((v_gb[i]->get_regs()->IF & v_gb[i]->get_regs()->IE & INT_SERIAL)) return false;
	}
	return true;

}


void hack_4p_burger_time_deluxe::process() {


	if (mem.game_state != CLEAR_SCREEN) {

		if (v_gb[0]->get_cpu()->total_clock < mem.seri_occer) return;
		mem.seri_occer = v_gb[0]->get_cpu()->total_clock + mem.transfer_speed;
		if (!is_ready_to_process()) return;

	}

	switch (mem.game_state)
	{
	case TITLE_SCREEN:
	{

		if (!mem.send_data_queue.empty())
		{
			byte data = mem.send_data_queue.front();
			mem.send_data_queue.pop();
			broadcast_byte(data);
			if (mem.send_data_queue.empty())
			{
				mem.game_state = HANDICAP_SCREEN;
				fill_send_data_queue({0,0,0,0,0,0,0,0x08,0,0xf0})
			}

		}
		break;

	}
	case HANDICAP_SCREEN:
	{
		/*
		if (++process_counter < 5) return 0;
		process_counter = 0;
		*/
		//if (!is_ready_to_process()) return;

		if (!mem.send_data_queue.empty())
		{
			byte data = mem.send_data_queue.front();
			mem.send_data_queue.pop();
			broadcast_byte(data);
			if (mem.send_data_queue.empty())
			{
				mem.game_state = HEIGHT_SELECT;
				generate_height_blocks();
			}

		}
		break;
	}
	case HEIGHT_SELECT:
	{
		/*
		if (++process_counter < 5 && !is_ready_to_process()) return 0;
		process_counter = 0;
		*/
		mem.transfer_speed = 4096;

		if (!mem.out_height_blocks_queue.empty())
		{
			byte data = mem.out_height_blocks_queue.front();
			mem.out_height_blocks_queue.pop();
			broadcast_byte(data);
			if (mem.out_height_blocks_queue.empty())
			{
				generate_falling_blocks();
			}
			break;
		}
		if (!mem.out_falling_blocks_queue.empty())
		{
			byte data = mem.out_falling_blocks_queue.front();
			mem.out_falling_blocks_queue.pop();
			broadcast_byte(data);
			if (mem.out_falling_blocks_queue.empty())
			{
				mem.game_state = IN_GAME;
			}
			break;
		}
		break;
	}
	case IN_GAME:
	{

		mem.transfer_speed = 4096 * 1024 / 8;
		/*
		if (++process_counter < 5) return 0;
		process_counter = 0;
		if (!is_ready_to_process()) return 0;
		*/


		get_all_SC_reg_data();
		handle_ingame_data();
		break;

	}
	case WINNER_SCREEN:
	{

		//wait 5 Seconds
		if (++mem.process_counter < 154 * 60 * 5) return;
		mem.process_counter = 0;

		// would be better to handle actually answers
		mem.send_data_queue.push(0x60);
		mem.send_data_queue.push(0x02);
		mem.send_data_queue.push(0x02);
		mem.send_data_queue.push(0x02);
		mem.send_data_queue.push(0x79);
		//mem.send_data_queue.push(0x29);   //start sending height blocks

		clear_data_for_next_round();
		mem.game_state = START_NEXT;

		break;
	}
	case START_NEXT:
	{
		/*
		if (++process_counter < 5 && !is_ready_to_process()) return 0;
		process_counter = 0;
		*/

		if (!mem.send_data_queue.empty())
		{
			byte data = mem.send_data_queue.front();
			mem.send_data_queue.pop();
			broadcast_byte(data);

			break;
		}


		get_all_SB_reg_data();

		if (is_expected_data(0x55))
		{

			mem.game_state = HEIGHT_SELECT;
			generate_height_blocks();

		}
		broadcast_byte(0x29);

		break;
	}

	default:
		break;
	}


	//seri_occer = v_gb[0]->get_cpu()->total_clock + transfer_speed;
}



/*
void dmg07::serialize(serializer& s)
{

	s_VAR(current_state);
	s_VAR(transfer_speed);
	s_VAR(seri_occer);
	s_VAR(transfer_count);
	s_VAR(phase_byte_count);
	s_VAR(restart_in);
	s_VAR(enter_statu
		s_VAR(packet_size);
	s_VAR(transfer_rate);
	s_VAR(first_aa_trans_nr);
	s_VAR(sync_trans_nr);
	s_VAR(delay);
	s_VAR(ready_to_sync_master);
	s_VAR(master_is_synced);

	s_ARRAY(in_data_buffer);
	s_ARRAY(trans_buffer.data());
	s_ARRAY(ans_buffer.data());
	s_ARRAY(bytes_to_send.data());


}

*/

