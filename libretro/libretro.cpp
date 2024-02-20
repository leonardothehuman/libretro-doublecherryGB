#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1 // for fopencookie hack in serialize_size
#endif

#include <stdlib.h>

#include "libretro.h"
#include "../gb_core/gb.h"
#include "dmy_renderer.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "libretro.h"

#include "../gb_core/sio/sio_devices.hpp"
#include "inline/inline_variables.h"
#include "inline/inline_functions.h"



static void check_variables(void);

void retro_get_system_info(struct retro_system_info *info)
{
   info->library_name     = "DoubleCherryGB";
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
   info->library_version  = "v0.1.0" GIT_VERSION;
   info->need_fullpath    = false;
   info->valid_extensions = "gb|dmg|gbc|cgb|sgb";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   
    //TODO FIX 4-Player Splitscreen at start

   int w = 160, h = 144;
   info->geometry.max_width = w * max_gbs; // changed w * 4
   info->geometry.max_height = h * max_gbs; // chaned w * 4

   
  // if (emulated_gbs < 5 && _show_player_screens == 2)
   {
      // screen orientation for dual gameboy mode
       if (_screen_4p_split)
       {
           h *= 1;
           w *= 1; 
       }
       else if(_screen_vertical)
         h *= emulated_gbs;
       else
         w *= emulated_gbs;
   }
   /*
   else if (emulated_gbs < 10 && _show_player_screens == 2)
   {
       // screen orientation for dual gameboy mode
       if (_screen_4p_split)
       {
           h *= 1;
           w *= 1;
       }
       else if (_screen_vertical)
           h *= emulated_gbs;
       else
           w *= emulated_gbs;
   }
   else if (emulated_gbs <= 16 && _show_player_screens == 2)
   {
       // screen orientation for dual gameboy mode
       if (_screen_4p_split)
       {
           h *= 1;
           w *= 1;
       }
       else if (_screen_vertical)
           h *= emulated_gbs;
       else
           w *= emulated_gbs;
   }
   */

   info->timing.fps            = 4194304.0 / 70224.0;
   info->timing.sample_rate    = 44100.0f;
   info->geometry.base_width   = w;
   info->geometry.base_height  = h;
   info->geometry.aspect_ratio = float(w) / float(h);
   memcpy(my_av_info, info, sizeof(*my_av_info));
   
}


void retro_init(void)
{

   unsigned level = 4;
   struct retro_log_callback log;

   if(environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   environ_cb(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &level);

   if (environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
      libretro_supports_bitmasks = true;

   environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars_quad);
   check_variables();
}

void retro_deinit(void)
{
   libretro_supports_bitmasks          = false;
}

bool retro_load_game(const struct retro_game_info* info)
{
    size_t rom_size;
    byte* rom_data;
    const struct retro_game_info_ext* info_ext = NULL;
    //environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars_single);
    //environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars_quad);
        check_variables();

   mode = MODE_SINGLE_GAME;

    unsigned i;

    struct retro_input_descriptor desc[] = {
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
       { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },

       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
       { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },

       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 5, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 6, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 7, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 8, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 9, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 10, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 11, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 12, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 13, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 14, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
       { 15, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

       { 0 },

    };

    if (!info)
        return false;


    v_gb.clear();
    render.clear();

    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);


    switch (emulated_gbs)
    {
    case 1: environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars_single); break;
    case 2: environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars_dual); break;
    case 3: environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars_tripple); break;
    case 4: environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars_quad); break;

    }

 
    if (environ_cb(RETRO_ENVIRONMENT_GET_GAME_INFO_EXT, &info_ext) &&
        info_ext->persistent_data)
    {
        rom_data = (byte*)info_ext->data;
        rom_size = info_ext->size;
        libretro_supports_persistent_buffer = true;
    }
    else
    {
        rom_data = (byte*)info->data;
        rom_size = info->size;
    }


    //create gameboy instances
    for (byte i = 0; i < max_gbs; i++)
    {
        render.emplace_back(new dmy_renderer(i));
        v_gb.emplace_back(new gb(render[i], true, true));
        _serialize_size[i] = 0;
    }


 

   // size_t* save_size;
  // char* save_file[2];
  // save_file[0] = read_file_to_buffer("C:\\Users\\TimZen\\Downloads\\RetroArch\\RetroArch-Win64\\saves\\Pokemon - Red Version (USA, Europe) (SGB Enhanced).srm", save_size);
   //save_file[1] = read_file_to_buffer("C:\\Users\\TimZen\\Downloads\\RetroArch\\RetroArch-Win64\\saves\\Pokemon - Red Version (USA, Europe) (SGB Enhanced).srm", save_size);
 //  save_file[1] = read_file_to_buffer("..//saves//Pokemon - Red Version (USA, Europe) (SGB Enhanced).srm", save_size[1]);

 
   // load roms
   for (byte i = 0; i < max_gbs; i++)
   {
       /*
       //if (!save_file[i])
       if (false)
       {
           if (!v_gb[i]->load_rom(rom_data, rom_size, (byte*)save_file[i], *save_size,
               libretro_supports_persistent_buffer))
               return false;
       }
       else*/
       {
           if (!v_gb[i]->load_rom(rom_data, rom_size, NULL, 0,
               libretro_supports_persistent_buffer))
               return false;
       }
       v_gb[i]->set_use_gba(detect_gba);
 
   }

   set_cart_name(rom_data);

//   master_link = new dmg07(v_gb);

 
   //set link connections
   switch (emulated_gbs)
   {
       case 1:
       {
           auto_config_1p_link();
           mode = MODE_SINGLE_GAME; break; 
       }
       case 2:
       {
           //mode = MODE_DUAL_GAME;
           mode = MODE_SINGLE_GAME_DUAL;
           // for link cables and IR:
           //if (gblink_enable) {
               v_gb[0]->set_target(v_gb[1]);
               v_gb[1]->set_target(v_gb[0]);
           //}
           break;

       }
       case 3:
       {
           mode = MODE_SINGLE_GAME_DUAL;
           if (!master_link)  master_link = new dmg07(v_gb);
            auto_config_4p_hack();

           //master_link = new dmg07(v_gb); 
           //master_link = new tetris_4p_hack(v_gb);
           break;
       }

       case 4:
       {
           mode = MODE_SINGLE_GAME_DUAL;
        
           if (use_multi_adapter && !master_link) master_link = new dmg07(v_gb);
           auto_config_4p_hack();

           if (!use_multi_adapter && gblink_enable) 
           {
               v_gb[0]->set_target(v_gb[1]);
               v_gb[1]->set_target(v_gb[0]);
               v_gb[2]->set_target(v_gb[3]);
               v_gb[3]->set_target(v_gb[2]);
           }
           
           break;
       }
       case 5:
       case 6:
       case 7:
       case 8:
       case 9:
       case 10:
       case 11:
       case 12:
       case 13:
       case 14:
       case 15:
       case 16:
       {
          if (!strcmp(cart_name, "FACEBALL 2000   "))
           {
               delete master_link;
               master_link = NULL;
               linked_target_device = new faceball2000_cable(v_gb);
               v_gb[0]->set_linked_target(linked_target_device);

           }

           break;
       }
   }

     check_variables();

   return true;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num_info)
{

    /*
    if (type != RETRO_GAME_TYPE_GAMEBOY_LINK_2P)
        return false; // all other types are unhandled for now 


    // implement for 3 - 4 Player

    
   environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void *)vars_dual);
   unsigned i;

   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "Prev Audio Mode" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "Next Audio Mode" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },

      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "Prev Audio Mode" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "Next Audio Mode" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

      { 0 },
   };

   if (!info)
      return false;

   for (i = 0; i < 2; i++)
   {
      v_gb[i]   = NULL;
      render[i] = NULL;
   }

   check_variables();

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

   render[0] = new dmy_renderer(0);
   v_gb[0]   = new gb(render[0], true, true);
   if (!v_gb[0]->load_rom((byte*)info[0].data, info[0].size, NULL, 0, false))
      return false;

   for (i = 0; i < 2; i++)
      _serialize_size[i] = 0;

   if (gblink_enable)
   {
      render[1] = new dmy_renderer(1);
      v_gb[1] = new gb(render[1], true, true);

      if (!v_gb[1]->load_rom((byte*)info[1].data, info[1].size, NULL, 0,
               false))
         return false;

      // for link cables and IR:
      v_gb[0]->set_target(v_gb[1]);
      v_gb[1]->set_target(v_gb[0]);
   }

   mode = MODE_DUAL_GAME;
   return true;

   */
   
}

void retro_unload_game(void)
{
   unsigned i;
   for(i = 0; i < emulated_gbs; ++i) 
   {
      if (v_gb[i])
      {
         delete v_gb[i];
         v_gb[i] = NULL;
         delete render[i];
         render[i] = NULL;
      }
   }
   free(my_av_info);
   libretro_supports_persistent_buffer = false;
}

void retro_reset(void)
{
   for(int i = 0; i < emulated_gbs; ++i)
   {
      if (v_gb[i])
         v_gb[i]->reset(); 
   }
   if (master_link) master_link->reset();
}

void retro_run(void)
{
   bool updated = false;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();

   input_poll_cb();

    
   for (int line = 0; line < 154; line++)
   {
       for (int i = 0; i < emulated_gbs; i++)
       {
           v_gb[i]->run();
       }
       if (master_link) master_link->process();
   }
  
   /*
   //GBREMIXER test condition link collectef first ruppee
   if (v_gb[0]->get_cpu()->get_ram()[0x1B5E] == 1)
   {
      
       //open new save savestate test
       // open the file
       const char* savestate_filename = "C:\\Users\\TimZen\\Downloads\\RetroArch\\RetroArch-Win64\\\\states\\Legend of Zelda, The - Link's Awakening DX (USA, Europe) (Rev 2) (SGB Enhanced) (GB Compatible).state2";
       size_t state_size;

       // Read the savestate into a buffer
       char* state_buffer = read_file_to_buffer(savestate_filename, &state_size);
       if (state_buffer == NULL) {
           return;
       }
       retro_reset();
     
       // Unserialize the savestate using Libretro API
       if (!retro_unserialize(state_buffer, retro_serialize_size())) {
           printf("Failed to unserialize the savestate\n");
           free(state_buffer);
           return;
       }

       // Successfully unserialized the savestate, now you can proceed with emulation

       // Free the allocated buffer
       free(state_buffer);
       
      
     
   }
   */
}


void *retro_get_memory_data(unsigned id)
{
    switch(mode)
    {
        case MODE_SINGLE_GAME:
        case MODE_SINGLE_GAME_DUAL: /* todo: hook this properly */
        {
            switch(id)
            {
                case RETRO_MEMORY_SAVE_RAM:
                    return v_gb[id]->get_rom()->get_sram();
                case RETRO_MEMORY_RTC:
                    return &(render[id]->fixed_time);
                case RETRO_MEMORY_VIDEO_RAM:
                    return v_gb[id]->get_cpu()->get_vram();
                case RETRO_MEMORY_SYSTEM_RAM:
                    return v_gb[id]->get_cpu()->get_ram();
                default:
                    break;
            }
        }
        case MODE_DUAL_GAME:
        {
            switch(id)
            {
                case RETRO_MEMORY_GAMEBOY_1_SRAM:
                    return v_gb[id]->get_rom()->get_sram();
                case RETRO_MEMORY_GAMEBOY_1_RTC:
                    return &(render[id]->fixed_time);
                case RETRO_MEMORY_GAMEBOY_2_SRAM:
                    return v_gb[id]->get_rom()->get_sram();
                case RETRO_MEMORY_GAMEBOY_2_RTC:
                    return &(render[id]->fixed_time);
                default:
                    break;
            }
        }
    }
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    switch(mode)
    {
        case MODE_SINGLE_GAME:
        case MODE_SINGLE_GAME_DUAL: /* todo: hook this properly */
        {
            switch(id)
            {
                case RETRO_MEMORY_SAVE_RAM:
                    return v_gb[id]->get_rom()->get_sram_size();
                case RETRO_MEMORY_RTC:
                    return sizeof(render[id]->fixed_time);
                case RETRO_MEMORY_VIDEO_RAM:
                    if (v_gb[id]->get_rom()->get_info()->gb_type >= 3)
                        return 0x2000*2; //sizeof(cpu::vram);
                    return 0x2000;
                case RETRO_MEMORY_SYSTEM_RAM:
                    if (v_gb[id]->get_rom()->get_info()->gb_type >= 3)
                        return 0x2000*4; //sizeof(cpu::ram);
                    return 0x2000;
                default:
                    break;
            }
        }
        case MODE_DUAL_GAME:
        {
            switch(id)
            {
                case RETRO_MEMORY_GAMEBOY_1_SRAM:
                    return v_gb[id]->get_rom()->get_sram_size();
                case RETRO_MEMORY_GAMEBOY_1_RTC:
                    return sizeof(render[id]->fixed_time);
                case RETRO_MEMORY_GAMEBOY_2_SRAM:
                    return v_gb[id]->get_rom()->get_sram_size();
                case RETRO_MEMORY_GAMEBOY_2_RTC:
                    return sizeof(render[id]->fixed_time);
                default:
                    break;
            }
        }
    }
   return 0;
}



// question: would saving both gb's into the same file be desirable ever?
// answer: yes, it's most likely needed to sync up netplay and for bsv records.
size_t retro_serialize_size(void)
{
    /*
   if (!(_serialize_size[0] + _serialize_size[1]))
   {
       _serialize_size[0] = v_gb[0]->get_state_size();
       _serialize_size[1] = v_gb[0]->get_state_size();
*/
      //unsigned i;
    size_t _all_size = 0;
      //for(i = 0; i < emulated_gbs; ++i)
    //for(i = 0; i < emulated_gbs; ++i)
    for(int i = 0; i < max_gbs; ++i)
      {
        if (v_gb[i]) {
            _serialize_size[i] = v_gb[i]->get_state_size();
            _all_size += _serialize_size[i];
        }
      }
   //}
   //return _serialize_size[0] + _serialize_size[1];
    return _all_size + 0x300;
}

void log_save_state(uint8_t* data, size_t size)
{
    if (logging_allowed)
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




bool retro_serialize(void *data, size_t size)
{
 // if (size == retro_serialize_size())
   {
      unsigned i;
      uint8_t *ptr = (uint8_t*)data;
    
      for(int i = 0; i < max_gbs; ++i)
      {
         if (v_gb[i])
         {
            v_gb[i]->save_state_mem(ptr);
            ptr += _serialize_size[i];
         }
      }
      
      if(master_link) master_link->save_state_mem(ptr);

      return true;
   }
   return false;
}

bool retro_unserialize(const void *data, size_t size)
{
   
  //if (size == retro_serialize_size())
   {
      unsigned i;
      uint8_t *ptr = (uint8_t*)data;
     
      for(i = 0; i < max_gbs; ++i)
      {
         if (v_gb[i])
         {
            v_gb[i]->restore_state_mem(ptr);
            ptr += _serialize_size[i];
         }
      }
   
      if(master_link) master_link->restore_state_mem(ptr);
      return true;
   }
   return false;
}

void retro_cheat_reset(void)
{
   for(int i=0; i< emulated_gbs; ++i)
   {
      if(v_gb[i])
         v_gb[i]->get_cheat()->clear();
   }
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
#if 1==0
   if (log_cb)
      log_cb(RETRO_LOG_INFO, "CHEAT:  id=%d, enabled=%d, code='%s'\n", index, enabled, code);
   // FIXME: work in progress.
   // As it stands, it seems TGB Dual only has support for Gameshark codes.
   // Unfortunately, the cheat.xml that ships with bsnes seems to only have
   // Game Genie codes, which are ROM patches rather than RAM.
   // http://nocash.emubase.de/pandocs.htm#gamegeniesharkcheats
   if(false && g_gb[0])
   {
      cheat_dat cdat;
      cheat_dat *tmp=&cdat;

      strncpy(cdat.name, code, sizeof(cdat.name));

      tmp->enable = true;
      tmp->next = NULL;

      while(false)
      { // basically, iterate over code.split('+')
         // TODO: remove all non-alnum chars here
         if (false)
         { // if strlen is 9, game genie
            // game genie format: for "ABCDEFGHI",
            // AB   = New data
            // FCDE = Memory address, XORed by 0F000h
            // GIH  = Check data (can be ignored for our purposes)
            word scramble;
            sscanf(code, "%2hhx%4hx", &tmp->dat, &scramble);
            tmp->code = 1; // TODO: test if this is correct for ROM patching
            tmp->adr = (((scramble&0xF) << 12) ^ 0xF000) | (scramble >> 4);
         }
         else if (false)
         { // if strlen is 8, gameshark
            // gameshark format for "ABCDEFGH",
            // AB    External RAM bank number
            // CD    New Data
            // GHEF  Memory Address (internal or external RAM, A000-DFFF)
            byte adrlo, adrhi;
            sscanf(code, "%2hhx%2hhx%2hhx%2hhx", &tmp->code, &tmp->dat, &adrlo, &adrhi);
            tmp->adr = (adrhi<<8) | adrlo;
         }
         if(false)
         { // if there are more cheats left in the string
            tmp->next = new cheat_dat;
            tmp = tmp->next;
         }
      }
   }
   g_gb[0].get_cheat().add_cheat(&cdat);
#endif
}


// start boilerplate

unsigned retro_api_version(void) { return RETRO_API_VERSION; }
unsigned retro_get_region(void) { return RETRO_REGION_NTSC; }

void retro_set_controller_port_device(unsigned port, unsigned device) { }

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) { }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }


void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_system_content_info_override content_overrides[] = {
      {
         "gb|dmg|gbc|cgb|sgb", /* extensions */
         false,    /* need_fullpath */
         true      /* persistent_data */
      },
      { NULL, false, false }
   };
   environ_cb = cb;
   cb(RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO, (void*)subsystems);
   /* Request a persistent content data buffer */
   cb(RETRO_ENVIRONMENT_SET_CONTENT_INFO_OVERRIDE,
         (void*)content_overrides);
}

// end boilerplate
