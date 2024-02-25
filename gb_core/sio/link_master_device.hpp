#pragma once

#include "../gb.h"
#include "../serializer.h"

#include <fstream>

extern bool logging_allowed;

class link_master_device{

	
public:
	virtual void process() = 0;
	virtual void reset() = 0;
	void virtual save_state_mem(void* buf);
	void virtual restore_state_mem(void* buf) ;
	size_t virtual get_state_size();
	void virtual serialize(serializer& s) = 0;
	
	int transfer_speed = 512 * 8;
	int seri_occer = 512*8*5;

protected:
	virtual byte send_byte(byte which, byte dat);
	virtual void broadcast_byte(byte dat);
	void log_traffic(byte id, byte b);

	bool gbs_are_ready_to_process();
	bool is_ready_for_next_tik();
	void update_seri_occer();

	//virtual void handle_answer(int i, byte dat);

	
	//helper functions
	void get_all_SC_reg_data();
	void get_all_SB_reg_data();
	bool is_expected_data(byte data);
	bool all_IE_are_handled();

	byte in_data_buffer[16];
	std::vector<gb*> v_gb;

};

/*
enum hack_4p_burger_time_deluxe_state
{
	TITLE_SCREEN,
	HANDICAP_SCREEN,
	INGAME_SCREEN,
	CLEAR_SCREEN

};
*/

/*   
struct hack_4p_burger_time_deluxe_mem_state {

	int transfer_speed = 512 * 8;
	int seri_occer = 2048 * 2048 * 2048;

	byte in_data_buffer[4];
	std::queue<byte> send_data_queue;

// This method lets cereal know which data members to serialize
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			transfer_speed,
			seri_occer,
			in_data_buffer,
			game_state,
			send_data_queue
		); // serialize things by passing them to the archive
	}

};
*/
