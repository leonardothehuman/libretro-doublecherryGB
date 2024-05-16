## DoubleCherryGB

<img src="/screenshots/logo.png" height="250">



**author:** Tim Oelrichs<br>
**email:** timoelrichs@gmail.com<br>
**website:** [timoelrichs.is-a.dev](https://timoelrichs.is-a.dev)<br>
https://patreon.com/double_cherry_projects<br>
https://www.buymeacoffee.com/timoelrichs<br>


> DoubleCherryGB is an open source (GPLv2) GB/GBC emulator with up to 16 Player support, based on the TGBDual libretro core by GIGO and Hii.
> It emulates the GAMEBOY 4 PLAYER ADAPTER (DMG-07) and also adds 1-16 Player Link Hacks.
> It's named after the Double Cherry Item from Super Mario 3D World.

Check my [website](https://timoelrichs.is-a.dev) for more information, planned features and compatibility list.

<br>

### Features

- GAMEBOY 4 PLAYER ADAPTER (DMG-07) emulation (F1-Race, Wave Race, Yoshi's Cookie...)
- up to 16-Player Faceball 2000 emulation (1-15 without bugfix-patch, 16 players needs patched rom https://github.com/Zarithya/Faceball2000DX))
- up to 16-Player Tetris Battle Royal Hack (no modified rom needed)
- up to 16-Player Kwirk Multiplayer Hack (no modified rom needed)
- up to 16 Player 4x4Player Adapter
- Of course up to 16 Player Singleplayer for Speedrun-battles or if you have arguing kids :D
- local splitscreen
- netplay support
- BARCODEBOY Emulation full support (all barcodes are hardcoded and will autoload the next after scan)  
- easy Pokemon Gen1 and Gen2 online trading via Lobby without subsystem (Retroarch 1.17 needed) 

Netplay is working. Make sure to have all the same amount of emulated gameboys in your core options.


![](/screenshots/F-1%20Race.png)
![](/screenshots/SuperRC.png)
![](/screenshots/TopRankTennis.png)
![](/screenshots/YoshisCookie.png)
![](/screenshots/WaveRace.png)

### How to

You can download the core via RetroArch Online-Updater on Windows, Linux and Android.
To emulate up to 16 units, use the core options in your frontend.  

### Easy gen1/gen2 pokemon online trading

Why is this easier? 
You just load up one Gameboy and one rom. No subsystem requiered. 
You can find trading partners over the netplay lobby (netplay -> refresh Host List)
Also works in LAN.

Retroarch 1.17 is needed, because this feature uses the new netpacket api.
only the linkcable data is transfered over network, so you won't see the others players screen.

- set the emulated gameboys to 1 and start or join a netplay. 
- a second player can join any time

go to the cable club and talk to the cable club npc. 
after both players sat down on the table and you see the please wait! screen, you will experince an big slowdown (up to 15 sec)
this is because all the pokemondata is transfered. After that you should be able to trade pokemon.

If you like to support me
https://patreon.com/double_cherry_projects<br>
https://www.buymeacoffee.com/timoelrichs<br>

Thanks to
- Shonumi for doing the research and documentation on the dmg-07  https://shonumi.github.io/index.html
- GIGO and Hii for making TGBDual and the libretro port

![](/screenshots/Faceball%202000.png)
![](/screenshots/Tetris.png)
![](/screenshots/tetris_winner.png)
![](/screenshots/Kwirk.png)

Original sources from
- https://github.com/libretro/tgbdual-libretro
- http://gigo.retrogames.com/download.html#tgb-dual
- http://shinh.skr.jp/tgbdualsdl/



