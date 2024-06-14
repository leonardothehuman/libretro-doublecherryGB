/*--------------------------------------------------
   TGB Dual - Gameboy Emulator -
   Copyright (C) 2001  Hii

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

// libretro implementation of the renderer, should probably be renamed from dmy.

#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>

#include "dmy_renderer.h"
#include "../gb_core/gb.h"
#include "libretro.h"

extern std::vector<gb* > v_gb;
extern int emulated_gbs;
extern int max_gbs; 
extern int _number_of_local_screens;

extern retro_log_printf_t log_cb;
extern retro_video_refresh_t video_cb;
extern retro_audio_sample_batch_t audio_batch_cb;
extern retro_input_poll_t input_poll_cb;
extern retro_input_state_t input_state_cb;
extern retro_environment_t environ_cb;

extern bool gblink_enable;

extern int audio_2p_mode;

#define MSG_FRAMES 60
#define SAMPLES_PER_FRAME (44100/60)

extern bool _screen_vertical;
extern bool _screen_4p_split; 
extern bool _screen_switched; // set to draw player 2 on the left/top
extern bool libretro_supports_bitmasks;
extern int _show_player_screen; // 0 = p1 only, 1 = p2 only, 2 = both players

dmy_renderer::dmy_renderer(int which)
{
   which_gb = which;

   retro_pixel_format pixfmt = RETRO_PIXEL_FORMAT_RGB565;
   rgb565 = environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixfmt);

#ifndef FRONTEND_SUPPORTS_RGB565
   if (rgb565 && log_cb)
      log_cb(RETRO_LOG_INFO, "Frontend supports RGB565; will use that instead of XRGB1555.\n");
#endif
}

word dmy_renderer::map_color(word gb_col)
{
#ifndef SKIP_COLOR_CORRECTION
#ifndef FRONTEND_SUPPORTS_RGB565
   if(rgb565)
   {
#endif
      return ((gb_col&0x001f) << 11) |
         ((gb_col&0x03e0) <<  1) |
         ((gb_col&0x0200) >>  4) |
         ((gb_col&0x7c00) >> 10);
#ifndef FRONTEND_SUPPORTS_RGB565
   }
   return ((gb_col&0x001f) << 10) |
      ((gb_col&0x03e0)      ) |
      ((gb_col&0x7c00) >> 10);
#endif
#else
   return gb_col;
#endif
}

word dmy_renderer::unmap_color(word gb_col)
{
#ifndef SKIP_COLOR_CORRECTION
#ifndef FRONTEND_SUPPORTS_RGB565
   if(rgb565)
   {
#endif
      return ((gb_col&0x001f) << 10) |
         ((gb_col&0x07c0) >>  1) |
         ((gb_col&0xf800) >> 11);
#ifndef FRONTEND_SUPPORTS_RGB565
   }
   return ((gb_col&0x001f) << 10) |
      ((gb_col&0x03e0)      ) |
      ((gb_col&0x7c00) >> 10);
#endif
#else
   return gb_col;
#endif
}

void dmy_renderer::refresh() {
   static int16_t stream[SAMPLES_PER_FRAME*2];
   // static int16_t stream[SAMPLES_PER_FRAME];

   
   //if (v_gb[1] && gblink_enable)
   //if (emulated_gbs > 1)
   if(true)
   {
        /*
      // if dual gb mode
      if (audio_2p_mode == 2)
      {
         // mix down to one per channel (dual mono)
         int16_t tmp_stream[SAMPLES_PER_FRAME*2];
         this->snd_render->render(tmp_stream, SAMPLES_PER_FRAME);
         for(int i = 0; i < SAMPLES_PER_FRAME; ++i)
         {
            int l = tmp_stream[(i*2)+0], r = tmp_stream[(i*2)+1];
            stream[(i*2)+which_gb] = int16_t( (l+r) / 2 );
         }
      }
      else if (audio_2p_mode == which_gb)
      */

       if (audio_2p_mode == which_gb)
       {
           // only play gb 0 or 1
           
           this->snd_render->render(stream, SAMPLES_PER_FRAME);
           audio_batch_cb(stream, SAMPLES_PER_FRAME);
      
           memset(stream, 0, sizeof(stream));
       
       }
       if (which_gb >= (emulated_gbs-1))
       {
         // only do audio callback after both gb's are rendered.
         //audio_batch_cb(stream, SAMPLES_PER_FRAME);

         //audio_2p_mode &= 3;
         //memset(stream, 0, sizeof(stream));
        }
   }
   else
   {
      this->snd_render->render(stream, SAMPLES_PER_FRAME);
      audio_batch_cb(stream, SAMPLES_PER_FRAME); 
   }
   fixed_time = time(NULL);

   
}

int dmy_renderer::check_pad()
{
   int16_t joypad_bits;
   if (libretro_supports_bitmasks)
      joypad_bits = input_state_cb(which_gb, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
   else
   {
      unsigned i;
      joypad_bits = 0;
      for (i = 0; i < (RETRO_DEVICE_ID_JOYPAD_R3 + 1); i++)
         joypad_bits |= input_state_cb(which_gb, RETRO_DEVICE_JOYPAD, 0, i) ? (1 << i) : 0;
   }

   // update pad state: a,b,select,start,down,up,left,right
   return
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_A))      ? 1 : 0) << 0 |
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_B))      ? 1 : 0) << 1 |
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) ? 1 : 0) << 2 |
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_START))  ? 1 : 0) << 3 |
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))   ? 1 : 0) << 4 |
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_UP))     ? 1 : 0) << 5 |
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))   ? 1 : 0) << 6 |
      ((joypad_bits & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))  ? 1 : 0) << 7;
}

void dmy_renderer::render_screen(byte* buf, int width, int height, int depth)
{
    static byte joined_buf[160*144*2*2]; // two screens' worth of 16-bit data
    static byte joined_buf3[160 * 144 * 3 * 2]; // three screens' worth of 16-bit data
    static byte joined_buf4[160 * 144 * 4 * 2]; // four screens' worth of 16-bit data
    static byte joined_buf8[160 * 144 * 8 * 2];
    static byte joined_buf9[160 * 144 * 9 * 2]; // three screens' worth of 16-bit data
    static byte joined_buf16[160 * 144 * 16 * 2];

    const int size_single_screen = sizeof(joined_buf) / 2;

    int pitch = width * ((depth + 7) / 8);
    int switched_gb = which_gb;
    if (_screen_switched)
        switched_gb = 1 - switched_gb;

   
    if (_number_of_local_screens == 1 || _show_player_screen == emulated_gbs)
    {
        switch (emulated_gbs)
        {
        case 1: video_cb(buf, width, height, pitch); break;
        case 2:
        {
            // are we drawing both gb's to the screen?
            if (_show_player_screen == emulated_gbs)
            {
                if (_screen_vertical)
                {
                    memcpy(joined_buf + switched_gb * size_single_screen, buf, size_single_screen);
                    if (which_gb == 1)
                        video_cb(joined_buf, width, height * 2, pitch);
                }
                else
                {
                    for (int row = 0; row < height; ++row)
                        memcpy(joined_buf + pitch * (2 * row + switched_gb), buf + pitch * row, pitch);
                    if (which_gb == 1)
                        video_cb(joined_buf, width * 2, height, pitch * 2);
                }

            }
            else
            {
                // are we currently on the gb that we want to draw?
                // (this ignores the "switch player screens" setting)
                if (_show_player_screen == which_gb)
                    memcpy(joined_buf, buf, size_single_screen);
                if (which_gb == (emulated_gbs - 1))
                    video_cb(joined_buf, width, height, pitch);
            }

            break;
        }
        case 3:
        {
            if (_show_player_screen == emulated_gbs) {
                if (_screen_4p_split)
                {
                    if (which_gb < 2) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf4 + pitch * (2 * row + switched_gb), buf + pitch * row, pitch);
                    }
                    else if (which_gb < 4) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf + pitch * (2 * row + switched_gb), buf + pitch * row, pitch);
                    }
                    if (which_gb == 2) {
                        memcpy(joined_buf4 + sizeof(joined_buf), joined_buf, sizeof(joined_buf));
                        video_cb(joined_buf4, width * 2, height * 2, pitch * 2);
                    }
                }
                else if (_screen_vertical)
                {
                    memcpy(joined_buf3 + switched_gb * size_single_screen, buf, size_single_screen);
                    if (which_gb == 2)
                        video_cb(joined_buf3, width, height * 3, pitch);
                }
                else
                {
                    for (int row = 0; row < height; ++row)
                        memcpy(joined_buf3 + pitch * (3 * row + switched_gb), buf + pitch * row, pitch);
                    if (which_gb == 2)
                        video_cb(joined_buf3, width * 3, height, pitch * 3);
                }
            }
            else
            {
                // are we currently on the gb that we want to draw?
                // (this ignores the "switch player screens" setting)
                if (_show_player_screen == which_gb)
                    memcpy(joined_buf, buf, size_single_screen);
                if (which_gb == emulated_gbs)
                    video_cb(joined_buf, width, height, pitch);
            }


            break;
        }
        case 4:
        {
            if (_show_player_screen == emulated_gbs) {

                if (_screen_4p_split)
                {
                    if (which_gb < 2) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf4 + pitch * (2 * row + switched_gb), buf + pitch * row, pitch);
                    }
                    else if (which_gb < 4) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf + pitch * (2 * row + switched_gb), buf + pitch * row, pitch);
                    }
                    if (which_gb == 3) {
                        memcpy(joined_buf4 + sizeof(joined_buf), joined_buf, sizeof(joined_buf));
                        video_cb(joined_buf4, width * 2, height * 2, pitch * 2);
                    }
                }

                else if (_screen_vertical)
                {
                    memcpy(joined_buf4 + switched_gb * size_single_screen, buf, size_single_screen);
                    if (which_gb == emulated_gbs - 1)
                        video_cb(joined_buf4, width, height * emulated_gbs, pitch);
                }
                else
                {
                    for (int row = 0; row < height; ++row)
                        memcpy(joined_buf4 + pitch * (emulated_gbs * row + switched_gb), buf + pitch * row, pitch);
                    if (which_gb == emulated_gbs - 1)
                        video_cb(joined_buf4, width * emulated_gbs, height, pitch * emulated_gbs);
                }
            }
            else
            {
                // are we currently on the gb that we want to draw?
                // (this ignores the "switch player screens" setting)
                if (_show_player_screen == which_gb)
                    memcpy(joined_buf, buf, size_single_screen);
                if (which_gb == (emulated_gbs - 1))
                    video_cb(joined_buf, width, height, pitch);
            }

            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        {
            if (_show_player_screen == emulated_gbs)
            {
                if (_screen_4p_split)
                {
                    if (which_gb < 3) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf9 + pitch * (3 * row + switched_gb), buf + pitch * row, pitch);
                    }
                    else if (which_gb < 6) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf3 + pitch * (3 * row + (switched_gb % 3)), buf + pitch * row, pitch);
                        if (which_gb == 5 || which_gb == (emulated_gbs - 1)) {
                            memcpy(joined_buf9 + (sizeof(joined_buf3)), joined_buf3, sizeof(joined_buf3));
                            memset(joined_buf3, 0, sizeof(joined_buf3));
                        }

                    }
                    else if (which_gb < 9) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf3 + pitch * (3 * row + (switched_gb % 3)), buf + pitch * row, pitch);
                        if (which_gb == (emulated_gbs - 1)) {
                            memcpy(joined_buf9 + (sizeof(joined_buf3) * 2), joined_buf3, sizeof(joined_buf3));

                        }
                    }

                    if (which_gb == (emulated_gbs - 1)) {
                        //memcpy(joined_buf16 + sizeof(joined_buf8), joined_buf8, sizeof(joined_buf8));
                        video_cb(joined_buf9, width * 3, height * 3, pitch * 3);
                    }
                }

                else if (_screen_vertical)
                {
                    memcpy(joined_buf16 + switched_gb * size_single_screen, buf, size_single_screen);
                    if (which_gb == emulated_gbs - 1)
                        video_cb(joined_buf16, width, height * emulated_gbs, pitch);
                }
                else
                {
                    for (int row = 0; row < height; ++row)
                        memcpy(joined_buf16 + pitch * (emulated_gbs * row + switched_gb), buf + pitch * row, pitch);
                    if (which_gb == emulated_gbs - 1)
                        video_cb(joined_buf16, width * emulated_gbs, height, pitch * emulated_gbs);
                }
            }
            else
            {
                // are we currently on the gb that we want to draw?
                // (this ignores the "switch player screens" setting)
                if (_show_player_screen == which_gb)
                    memcpy(joined_buf, buf, size_single_screen);
                if (which_gb == (emulated_gbs - 1))
                    video_cb(joined_buf, width, height, pitch);
            }

            break;
        }
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        {
            if (_show_player_screen == emulated_gbs) {


                if (_screen_4p_split)
                {
                    if (which_gb < 4) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf16 + pitch * (4 * row + switched_gb), buf + pitch * row, pitch);
                    }
                    else if (which_gb < 8) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf4 + pitch * (4 * row + (switched_gb % 4)), buf + pitch * row, pitch);
                        if (which_gb == 7) {
                            memcpy(joined_buf16 + (sizeof(joined_buf4)), joined_buf4, sizeof(joined_buf4));
                            memset(joined_buf4, 0, sizeof(joined_buf4));
                        }

                    }
                    else if (which_gb < 12) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf4 + pitch * (4 * row + (switched_gb % 4)), buf + pitch * row, pitch);
                        if (which_gb == 11 || which_gb == (emulated_gbs - 1)) {
                            memcpy(joined_buf16 + (sizeof(joined_buf4) * 2), joined_buf4, sizeof(joined_buf4));
                            memset(joined_buf4, 0, sizeof(joined_buf4));
                        }
                        /*
                        if (which_gb == (emulated_gbs - 1)) {
                            //memcpy(joined_buf16 + sizeof(joined_buf8), joined_buf8, sizeof(joined_buf8));
                            video_cb(joined_buf16, width * 4, height * 4, pitch * 4);
                        }*/

                    }
                    else if (which_gb < 16) {
                        for (int row = 0; row < height; ++row)
                            memcpy(joined_buf4 + pitch * (4 * row + (switched_gb % 4)), buf + pitch * row, pitch);
                        if (which_gb == (emulated_gbs - 1))
                            memcpy(joined_buf16 + (sizeof(joined_buf4) * 3), joined_buf4, sizeof(joined_buf4));
                    }
                    if (which_gb == (emulated_gbs - 1)) {
                        //memcpy(joined_buf16 + sizeof(joined_buf8), joined_buf8, sizeof(joined_buf8));
                        video_cb(joined_buf16, width * 4, height * 4, pitch * 4);
                    }
                }

                else if (_screen_vertical)
                {
                    memcpy(joined_buf16 + switched_gb * size_single_screen, buf, size_single_screen);
                    if (which_gb == emulated_gbs - 1)
                        video_cb(joined_buf16, width, height * emulated_gbs, pitch);
                }
                else
                {
                    for (int row = 0; row < height; ++row)
                        memcpy(joined_buf16 + pitch * (emulated_gbs * row + switched_gb), buf + pitch * row, pitch);
                    if (which_gb == emulated_gbs - 1)
                        video_cb(joined_buf16, width * emulated_gbs, height, pitch * emulated_gbs);
                }
            }
            else
            {
                // are we currently on the gb that we want to draw?
                // (this ignores the "switch player screens" setting)
                if (_show_player_screen == which_gb)
                    memcpy(joined_buf, buf, size_single_screen);
                if (which_gb == (emulated_gbs - 1))
                    video_cb(joined_buf, width, height, pitch);
            }


            break;
        }
        }
    }
    else 
    {
        switch (_number_of_local_screens) {
        case 2:
        {
          
            if (which_gb == _show_player_screen || which_gb == _show_player_screen+1)
            {
                if (_screen_vertical)
                {
                    memcpy(joined_buf + switched_gb * size_single_screen, buf, size_single_screen);
                    if (which_gb == _show_player_screen + 1)
                        video_cb(joined_buf, width, height * 2, pitch);
                }
                else
                {
                    for (int row = 0; row < height; ++row)
                        memcpy(joined_buf + pitch * (2 * row + switched_gb), buf + pitch * row, pitch);
                    if (which_gb == _show_player_screen + 1)
                        video_cb(joined_buf, width * 2, height, pitch * 2);
                }

            }
          
            break;
        }
        }
    }
}

byte dmy_renderer::get_time(int type)
{
   dword now = fixed_time-cur_time;

   switch(type)
   {
      case 8: // second
         return (byte)(now%60);
      case 9: // minute
         return (byte)((now/60)%60);
      case 10: // hour
         return (byte)((now/(60*60))%24);
      case 11: // day (L)
         return (byte)((now/(24*60*60))&0xff);
      case 12: // day (H)
         return (byte)((now/(256*24*60*60))&1);
   }
   return 0;
}

void dmy_renderer::set_time(int type,byte dat)
{
   dword now = fixed_time;
   dword adj = now - cur_time;

   switch(type)
   {
      case 8: // second
         adj = (adj/60)*60+(dat%60);
         break;
      case 9: // minute
         adj = (adj/(60*60))*60*60+(dat%60)*60+(adj%60);
         break;
      case 10: // hour
         adj = (adj/(24*60*60))*24*60*60+(dat%24)*60*60+(adj%(60*60));
         break;
      case 11: // day (L)
         adj = (adj/(256*24*60*60))*256*24*60*60+(dat*24*60*60)+(adj%(24*60*60));
         break;
      case 12: // day (H)
         adj = (dat&1)*256*24*60*60+(adj%(256*24*60*60));
         break;
   }
   cur_time = now - adj;
}

