#include "../gb.h"
#include "../serializer.h"

extern bool logging_allowed; 

class hack_4p_kwirk : public link_target {

public:
	hack_4p_kwirk(std::vector<gb*> g_gb);
	byte seri_send(byte data) override;
	byte get_SB_value() override { return 0xFF;};
	byte get_SC_value() override { return 0x80; };
	void reset();

	enum ingame_state {
		TITLE_SCREEN,
		SELECT_SCREEN,
		SELECT_SKILL,
		SELECT_COURSE_RMS,
		SELECT_CONTEST,
		SELECT_DISPLAY,
		SELECT_START,
		INGAME,
		WINNER_SCREEN,
		NEXT_MATCH_SCREEN
	};

private:
	void get_all_SB_reg_data() ;
	void get_all_SC_reg_data();
	void redirect_data(byte data);


	byte get_max_level();
	byte get_second_level();
	byte send_current_levels();

	byte get_winner_id(byte data);
	byte handle_winner(int id);
	void handle_loser();


	void log_traffic(byte id, byte b);

	std::vector<gb*> v_gb;
	byte in_data_buffer[4] = { 0,0,0,0 };;
	int master_id; 
	ingame_state current_state; 
};
