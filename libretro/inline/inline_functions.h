
void auto_config_4p_hack(byte* rombuf)
{
    char cart_name[18];
    memcpy(cart_name, rombuf + 0x134, 16);
    cart_name[16] = '\0';
    cart_name[17] = '\0';

    if (! strcmp(cart_name, "TETRIS"))
    {
        master_link = new hack_4p_tetris(v_gb);
    }
};


char* read_file_to_buffer(const char* filename, size_t* file_size) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    // Allocate memory for the buffer to hold the file contents
    char* buffer = (char*)malloc(*file_size);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, *file_size, file);

    // Close the file
    fclose(file);

    if (bytes_read != *file_size) {
        printf("Failed to read the entire file\n");
        free(buffer);
        return NULL;
    }

    return buffer;
}


void add_new_player() {
    // v_gb.push_back(new gb)
}

void check_for_new_players() {

    for (int i = v_gb.size(); i < 4; i++)
    {
        int is_start_pressed = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
        if (is_start_pressed) add_new_player();
    }

}

static void check_variables(void)
{
    struct retro_variable var;

    var.key = "multitgb_emulated_gameboys";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
       
        if (!already_checked_options) { // only apply this setting on init
            if (!strcmp(var.value, "1"))
            {
                emulated_gbs = 1;
                mode = MODE_SINGLE_GAME;
            }
            else if (!strcmp(var.value, "2"))
                emulated_gbs = 2;
            else if (!strcmp(var.value, "3"))
                emulated_gbs = 3;
            else if (!strcmp(var.value, "4"))
                emulated_gbs = 4;

        }
    }

    if (emulated_gbs == 2)
    {
        var.key = "tgbdual_log_link";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {

            if (!strcmp(var.value, "On"))
                logging_allowed = true;
            else  logging_allowed = false;

        }

    }

    //TODO FOR 3PLAYERS
    if (emulated_gbs > 2) {
        // check 4Player Mode
        var.key = "multitgbt_gblink_device";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {

            if (!strcmp(var.value, "4-player adapter"))
            {
                use_multi_adapter = true;
                if (!master_link) master_link = new dmg07(v_gb);
            }
            else if (!strcmp(var.value, "2x2 - player link"))
            {
                use_multi_adapter = false;
                master_link = NULL;
            }
            else if (!strcmp(var.value, "tetris multiplayer hack"))
            {
                use_multi_adapter = false;
                use_tetris_4p_hack = true;
                if (!master_link)  master_link = new hack_4p_tetris(v_gb);
            }
        }
        else
            _screen_vertical = false;
    }

    // check whether screen placement is horz (side-by-side) or vert
    if (emulated_gbs > 1)
    {
        // check whether link cable mode is enabled
        var.key = "tgbdual_gblink_enable";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {
            if (!already_checked_options) { // only apply this setting on init
                if (!strcmp(var.value, "disabled"))
                    gblink_enable = false;
                else if (!strcmp(var.value, "enabled"))
                    gblink_enable = true;
            }
        }
        else
            gblink_enable = false;

        // check whether link cable mode is enabled
        var.key = "tgbdual_log_link";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {
            if (!strcmp(var.value, "On"))
                logging_allowed = true;
            else logging_allowed = false; 
        }


        var.key = "tgbdual_screen_placement";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {
            if (!strcmp(var.value, "left-right"))
            {
                _screen_vertical = false;
                _screen_4p_split = false;
            }
            else if (!strcmp(var.value, "top-down"))
            {
                _screen_vertical = true;
                _screen_4p_split = false;
            }
            else if (!strcmp(var.value, "4-Player Splitscreen"))
            {
                _screen_vertical = false;
                _screen_4p_split = true;
            }
        }
        else
            _screen_vertical = false;

        // check whether player 1 and 2's screen placements are swapped
        var.key = "tgbdual_switch_screens";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {
            if (!strcmp(var.value, "normal"))
                _screen_switched = false;
            else if (!strcmp(var.value, "switched"))
                _screen_switched = true;
        }
        else
            _screen_switched = false;

        // check whether to show both players' screens, p1 only, or p2 only
        var.key = "tgbdual_single_screen_mp";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {
            if (!strcmp(var.value, "both players"))
                _show_player_screens = 2;
            else if (!strcmp(var.value, "player 1 only"))
                _show_player_screens = 0;
            else if (!strcmp(var.value, "player 2 only"))
                _show_player_screens = 1;
        }
        else
            _show_player_screens = 2;


        int screenw = 160, screenh = 144;

        if (_screen_4p_split) {
            screenw *= 2;
            screenh *= 2;
        }
        else if (emulated_gbs > 1 && _show_player_screens == 2)
        {
            if (_screen_vertical)
                screenh *= emulated_gbs;
            else
                screenw *= emulated_gbs;
        }


        my_av_info->geometry.base_width = screenw;
        my_av_info->geometry.base_height = screenh;
        my_av_info->geometry.aspect_ratio = float(screenw) / float(screenh);

        already_checked_options = true;
        environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, my_av_info);


        // check whether which audio should play
        var.key = "tgbdual_audio_output";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        {
            if (!strcmp(var.value, "Game Boy #1"))
                audio_2p_mode = 0;
            else if (!strcmp(var.value, "Game Boy #2"))
                audio_2p_mode = 1;
            else if (!strcmp(var.value, "Game Boy #3"))
                audio_2p_mode = 2;
            else if (!strcmp(var.value, "Game Boy #4"))
                audio_2p_mode = 3;
        }
        else
            _screen_switched = false;

    }
    else {

        int screenw = 160, screenh = 144;

        my_av_info->geometry.base_width = screenw;
        my_av_info->geometry.base_height = screenh;
        my_av_info->geometry.aspect_ratio = float(screenw) / float(screenh);

        already_checked_options = true;
        environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, my_av_info);
    }
}
