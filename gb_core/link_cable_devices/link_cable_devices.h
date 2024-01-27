
extern class gb; 

struct save_state_mem;

class link_master_device {
public:
	virtual void process();
	virtual void reset();
;
public:
	int transfer_speed = 512 * 8;
	int seri_occer = 2048 * 2048 * 2048;

};

class link_netplay_support
{
public:
	void save_mem_state(void* buf);
	void restore_mem_state(void* buf);
	size_t get_state_size();

	save_mem_state mem;
};

class link_data_sender
{

protected:
	void send_byte(byte which, byte dat);
	void broadcast_byte(byte dat);
	
	void log_traffic(byte id, byte b);

	std::vector<gb*> v_gb;

};

class link_data_handler {
	virtual void handle_answer(int i, byte dat);

	//helper functions
	void get_all_SC_reg_data();
	void get_all_SB_reg_data();
	bool is_expected_data(byte data);
	bool all_IE_are_handled();

	

};

class link_4p_hack : public link_master_device, link_netplay_support, link_data_sender, link_data_handler{
};

//############ DMG-07 

enum dmg07_state
{
	PING_PHASE,
	SYNC_PHASE,
	TRANSMISSION_PHASE,
};

struct dmg07_speed_values {
	int ping_speed;
	int transmission_speed;
};

struct dmg07_mem_state : save_state_mem {

	dmg07_state current_state = PING_PHASE;

	int transfer_speed = 512 * 8;
	int seri_occer = 2048 * 2048 * 2048;

	int transfer_count = 0;
	int phase_byte_count = 0;

	int last_trans_nr[4];
	int restart_in = 0;
	byte enter_status = 0x00;

	byte packet_size = 0;
	byte transfer_rate = 0x00;

	int first_aa_trans_nr = 0;
	int sync_trans_nr = 0;

	int delay = 0;
	bool ready_to_sync_master = false;
	bool master_is_synced = false;

	byte in_data_buffer[4];

	std::vector<byte> trans_buffer[4];
	std::vector<byte> ans_buffer[4];
	std::queue<byte> bytes_to_send;



	// This method lets cereal know which data members to serialize
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			current_state,
			transfer_speed,
			seri_occer,
			transfer_count,
			phase_byte_count,
			restart_in,
			enter_status,
			packet_size,
			transfer_rate,
			first_aa_trans_nr,
			sync_trans_nr,
			delay,
			ready_to_sync_master,
			master_is_synced,
			in_data_buffer,
			trans_buffer,
			ans_buffer,
			bytes_to_send
		); // serialize things by passing them to the archive
	}


};

struct dmg07_mem_state_size {
	size_t size;
	// This method lets cereal know which data members to serialize
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			size
		); // serialize things by passing them to the archive
	}

};

class dmg07 : public link_master_device {

	friend class cpu;
public:

	dmg07(std::vector<gb*> g_gb);

	void process();
	bool is_ready_to_process();
	void reset();
	void save_mem_state(void* buf);
	void restore_mem_state(void* buf);
	size_t get_state_size();
	void log_save_state(char* data, size_t size);

private:

	void get_all_SC_reg_data();
	void get_all_SB_reg_data();
	bool is_expected_data(byte data);
	bool all_IE_are_handled();

	void handle_answer(int i, byte dat);
	void restart_pingphase();
	void log_traffic(byte id, byte b);

	void send_byte(byte which, byte dat);
	void broadcast_byte(byte dat);
	void send_sync_bytes();
	void send_each_ping_byte();
	void send_ping_byte(byte which);

	void fill_buffer_for_less_than_4p();
	void clear_all_buffers();

	std::vector<gb*> v_gb;
	dmg07_mem_state mem{};
};





enum tetris_4p_hack_state
{
	TITLE_SCREEN,
	MUSIC_SELECT,
	HEIGHT_SELECT,
	IN_GAME,
	WINNER_SCREEN,
	START_NEXT
};

enum tetris_4p_hack_player_state
{
	IS_ALIVE,
	IS_KO,
	IS_SENDING_LINES,
	IS_WINNER,
	IS_LOOSER,
	IS_IN_WINNER_SCREEN

};

struct tetris_4p_hack_lines_packet {
	byte lines;
	int from;

	// This method lets cereal know which data members to serialize
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			lines,
			from
		); // serialize things by passing them to the archive
	}

};

struct tetris_4p_hack_mem_state {

	int transfer_speed = 512 * 8;
	int seri_occer = 2048 * 2048 * 2048;

	byte in_data_buffer[4];
	std::queue<byte> out_height_blocks_queue;
	std::queue<byte> out_falling_blocks_queue;
	std::queue<byte> send_data_queue;
	std::queue<tetris_4p_hack_lines_packet> lines_queue;

	byte falling_block_choice[10] = {
		0x00,
		0x04,
		0x04,
		0x08,
		0x08,
		0x0c,
		0x0c,
		0x10,
		0x14,
		0x18
	};

	tetris_4p_hack_state tetris_state = TITLE_SCREEN;
	tetris_4p_hack_player_state players_state[4];
	byte current_max_height = 0;
	int lines_from_player_id = 0;
	byte lines_to_send[4] = { 0,0,0,0 };
	byte next_bytes_to_send[4] = { 0,0,0,0 };
	int process_counter = 0;
	int process_occer = 5;

	// This method lets cereal know which data members to serialize
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			in_data_buffer,
			out_height_blocks_queue,
			out_falling_blocks_queue,
			send_data_queue,
			lines_queue,
			falling_block_choice,
			tetris_state,
			players_state,
			current_max_height,
			lines_from_player_id,
			lines_to_send,
			next_bytes_to_send,
			process_counter,
			process_occer
		); // serialize things by passing them to the archive
	}

};

class tetris_4p_hack : public link_master_device {

	friend class cpu;
public:

	tetris_4p_hack(std::vector<gb*> g_gb);

	void process();
	bool is_ready_to_process();
	void reset();
	void save_mem_state(void* buf);
	void restore_mem_state(void* buf);
	size_t get_state_size();

private:
	void log_traffic(byte id, byte b);
	void init_send_data_queue();
	void clear_data_for_next_round();

	void generate_height_blocks();
	void generate_falling_blocks();

	void get_all_SC_reg_data();
	void get_all_SB_reg_data();
	bool is_expected_data(byte data);
	bool all_IE_are_handled();

	void broadcast_byte(byte data);
	void send_byte(byte which, byte data);

	void send_ingame_bytes();
	void handle_ingame_data();
	void update_ingame_states();
	int check_winner_id();
	bool all_are_in_winnerscreen();

	std::vector<gb*> v_gb;
	tetris_4p_hack_mem_state mem{};

};


enum burgertime_4p_hack_state
{
	TITLE_SCREEN,
	HANDICAP_SCREEN,
	INGAME_SCREEN,
	CLEAR_SCREEN

};

struct burgertime_4p_hack_mem_state {

	int transfer_speed = 512 * 8;
	int seri_occer = 2048 * 2048 * 2048;

	byte in_data_buffer[4];
	std::queue<byte> send_data_queue;
	burgertime_4p_hack_state game_state = TITLE_SCREEN;


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

class burgertime_4p_hack : public link_master_device, link_4p_hack_device, link_netplay_support {

	friend class cpu;
public:

	burgertime_4p_hack(std::vector<gb*> g_gb);

	void process();
	bool is_ready_to_process();
	void reset();
	void save_mem_state(void* buf);
	void restore_mem_state(void* buf);
	size_t get_state_size();

private:
	void log_traffic(byte id, byte b);
	void fill_send_data_queue(char sequence[]);
	void init_send_data_queue();
	void clear_data_for_next_round();

	void get_all_SC_reg_data();
	void get_all_SB_reg_data();
	bool is_expected_data(byte data);
	bool all_IE_are_handled();

	void broadcast_byte(byte data);
	void send_byte(byte which, byte data);

	void send_ingame_bytes();
	void handle_ingame_data();
	void update_ingame_states();
	int check_winner_id();
	bool all_are_in_winnerscreen();

	std::vector<gb*> v_gb;
	burgertime_4p_hack_mem_state mem{};

};

