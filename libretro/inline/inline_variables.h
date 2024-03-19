
#define RETRO_MEMORY_GAMEBOY_1_SRAM ((1 << 8) | RETRO_MEMORY_SAVE_RAM)
#define RETRO_MEMORY_GAMEBOY_1_RTC ((2 << 8) | RETRO_MEMORY_RTC)
#define RETRO_MEMORY_GAMEBOY_2_SRAM ((3 << 8) | RETRO_MEMORY_SAVE_RAM)
#define RETRO_MEMORY_GAMEBOY_2_RTC ((3 << 8) | RETRO_MEMORY_RTC)

#define RETRO_GAME_TYPE_GAMEBOY_LINK_2P 0x101

unsigned int num_clients = 0;
unsigned short my_client_id = -1; 
static retro_netpacket_send_t netpacket_send_fn_ptr = NULL;
static retro_netpacket_poll_receive_t netpacket_pollrcv_fn_ptr = NULL;



static const struct retro_variable vars_single[] = {
    { "multitgb_emulated_gameboys", "Number of emulated Gameboys (reload); 1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16" },
    // { "doublecherrygb_detect_gba", "detect playing on gba (gba enhancements); Off|On" },
    { NULL, NULL },
};

static const struct retro_variable vars_dual[] = {
    { "multitgb_emulated_gameboys", "Number of emulated Gameboys (reload); 1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16" },
    { "tgbdual_gblink_enable", "Link cable emulation (reload); disabled|enabled" },
    { "tgbdual_screen_placement", "Screen layout; left-right|top-down" },
   // { "tgbdual_switch_screens", "Switch player screens; normal|switched" },
    { "tgbdual_single_screen_mp", "Show player screens; all players|player 1 only|player 2 only" },
    { "tgbdual_audio_output", "Audio output; Game Boy #1|Game Boy #2" },
   // { "tgbdual_log_link", "Log Link Cable Communication; Off|On" },
    { NULL, NULL },
};

static const struct retro_variable vars_tripple[] = {
    { "multitgb_emulated_gameboys", "Number of emulated Gameboys (reload); 1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16" },
    { "tgbdual_gblink_enable", "Link cable emulation (reload); disabled|enabled" },
     { "multitgbt_gblink_device", "Link cable device (reload); 4-player adapter" },
     { "tgbdual_screen_placement", "Screen layout; left-right|4-Player Splitscreen|top-down" },
    //{ "tgbdual_switch_screens", "Switch player screens; normal|switched" },
    { "tgbdual_single_screen_mp", "Show player screens; all players|player 1 only|player 2 only|player 3 only" },
    { "tgbdual_audio_output", "Audio output; Game Boy #1|Game Boy #2|Game Boy #3" },
     //{ "tgbdual_log_link", "Log Link Cable Communication; Off|On" },
    { NULL, NULL },
};

static const struct retro_variable vars_quad[] = {
    { "multitgb_emulated_gameboys", "Number of emulated Gameboys (reload); 1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16" },
     { "tgbdual_gblink_enable", "Link cable emulation (reload); disabled|enabled" },
    { "multitgbt_gblink_device", "Link cable device (reload); 4-player adapter|2x2-player link" },
     { "tgbdual_screen_placement", "Screen layout; 4-Player Splitscreen" },
   // { "tgbdual_switch_screens", "Switch player screens; normal|switched" },
    { "tgbdual_single_screen_mp", "Show player screens; all players|player 1 only|player 2 only|player 3 only|player 4 only|player 5 only|player 6 only|player 7 only|player 8 only|player 9 only|player 10 only|player 11 only|player 12 only|player 13 only|player 14 only|player 15 only|player 16 only" },
     { "tgbdual_audio_output", "Audio output; Game Boy #1|Game Boy #2|Game Boy #3|Game Boy #4|Game Boy #5|Game Boy #6|Game Boy #7|Game Boy #8|Game Boy #9|Game Boy #10|Game Boy #11|Game Boy #12|Game Boy #13|Game Boy #14|Game Boy #15|Game Boy #16" },
   // { "tgbdual_log_link", "Log Link Cable Communication; Off|On" },
    { NULL, NULL },
};

static const struct retro_subsystem_memory_info gb1_memory[] = {
    { "srm", RETRO_MEMORY_GAMEBOY_1_SRAM },
    { "rtc", RETRO_MEMORY_GAMEBOY_1_RTC },
};

static const struct retro_subsystem_memory_info gb2_memory[] = {
    { "srm", RETRO_MEMORY_GAMEBOY_2_SRAM },
    { "rtc", RETRO_MEMORY_GAMEBOY_2_RTC },
};

static const struct retro_subsystem_rom_info gb_roms[] = {

    { "GameBoy #1", "gb|gbc", false, false, false, gb1_memory, 1 },
    { "GameBoy #2", "gb|gbc", false, false, false, gb2_memory, 1 },

};

 //subsystem not working, for now disable
static const struct retro_subsystem_info subsystems[] = {
  // { "2 Player Game Boy Link", "gb_link_2p", gb_roms, 2, RETRO_GAME_TYPE_GAMEBOY_LINK_2P },
   //{ "3 Player Game Boy with 4-Player Adapter", "gb_3p", gb_roms, 2, RETRO_GAME_TYPE_GAMEBOY_LINK_2P },
   //{ "4 Player Game Boy with 4-Player Adapter", "gb_4p", gb_roms, 2, RETRO_GAME_TYPE_GAMEBOY_LINK_2P },
   //{ "4 Player Game Boy 2x 2-Player Link", "gb_2x2p", gb_roms, 2, RETRO_GAME_TYPE_GAMEBOY_LINK_2P },
   { NULL },
};


enum mode {
    MODE_SINGLE_GAME,
    MODE_SINGLE_GAME_DUAL,
    MODE_DUAL_GAME
};

static enum mode mode = MODE_SINGLE_GAME;


std::vector<gb*> v_gb;
std::vector <dmy_renderer*> render;
link_master_device* master_link;
link_target* linked_target_device; 
bool use_multi_adapter = false;
bool use_tetris_4p_hack = false;
bool log_2p_link = false;
bool logging_allowed = false; 
bool detect_gba = false; 
int max_gbs = 16;


retro_log_printf_t log_cb;
retro_video_refresh_t video_cb;
retro_audio_sample_batch_t audio_batch_cb;
retro_environment_t environ_cb;
//retro_environment_t environ_cb;
retro_input_poll_t input_poll_cb;
retro_input_state_t input_state_cb;

bool _screen_vertical = false;
bool _screen_4p_split = false;
bool _screen_switched = false;
int _show_player_screens = 1;
static size_t _serialize_size[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

bool gblink_enable = false;
int emulated_gbs = 4;
char cart_name[18];

int audio_2p_mode = 0;
// used to make certain core options only take effect once on core startup
bool already_checked_options = false;
bool libretro_supports_persistent_buffer = false;
bool libretro_supports_bitmasks = false;
struct retro_system_av_info* my_av_info = (retro_system_av_info*)malloc(sizeof(*my_av_info));

unsigned libretro_msg_interface_version = 0;