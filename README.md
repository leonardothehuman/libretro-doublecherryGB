## DoubleCherryGB

<img src="/screenshots/logo.png" height="250">



**author:** Tim Oelrichs<br>
**email:** timoelrichs@gmail.com<br>
**website:** [timoelrichs.is-a.dev](https://timoelrichs.is-a.dev)<br>
https://patreon.com/double_cherry_projects<br>
https://www.buymeacoffee.com/timoelrichs<br>


> DoubleCherryGB is an open source (GPLv2) GB/GBC emulator with up to 16 Player support, based on the TGBDual libretro core by GIGO and Hii.
> It emulates the GAMEBOY 4 PLAYER ADAPTER (DMG-07) and also adds 3-16 Player Link Hacks for two-player games.
> It's named after the Double Cherry Item from Super Mario 3D World.

Check my [website](https://timoelrichs.is-a.dev) for more information, planned features and compatibility list.

<br>

### Features

- Link Cable over network, easy Pokemon Gen1 and Gen2 online trading via Lobby (Retroarch 1.17 needed) 
- GAMEBOY 4 PLAYER ADAPTER (DMG-07) emulation (F1-Race, Wave Race, Yoshi's Cookie...) [compatibility list](https://peppermint-theater-276.notion.site/4-player-adapter-compatibility-8d33abe17f1d4bf5a361b368af6622b7)
- up to 16-Player Faceball 2000 emulation (1-15 without bugfix-patch, 16 players needs [patched](https://github.com/Zarithya/Faceball2000DX) rom)
- up to 16-Player Tetris Battle Royal Hack (virtual link device, no modified rom needed)
- up to 16-Player Kwirk Multiplayer Hack (virtual link device, no modified rom needed)
- up to 16 Player 4x4Player Adapter
- Of course up to 16 Player Singleplayer for speedrun-battles or if you have arguing kids :D
- local splitscreen
- netplay support
- BARCODEBOY Emulation full support (all barcodes are hardcoded and will autoload the next after scan)  


Netplay is working. Make sure to have all the same amount of emulated gameboys in your core options.


![](/screenshots/F-1%20Race.png)
![](/screenshots/SuperRC.png)
![](/screenshots/TopRankTennis.png)
![](/screenshots/YoshisCookie.png)
![](/screenshots/WaveRace.png)

### How to

You can download the core via RetroArch Online-Updater on Windows, Linux and Android.
To emulate up to 16 units, use the core options in your frontend.  

<br>

### Link Cable over network
#### Easy gen1/gen2 pokemon online trading

As you may not know, the TGBdual and Sameboy Gameboy link cable subsystem is not meant for netplay and does only work on your local maschine.
There are workarounds with savefile exchange. [OnionOS](https://onionui.github.io/docs/multiplayer/easynetplay-pokemon) and Koriki CFW for the miyoo mini have features to automate this process.
But these require a supported retrohandheld and is only possible local via personal hotspot.
For online trading gambatte was the only option, because it has its own link over network implementation.

With RetroArch 1.17 we got a new netpacket API meant and made for DOSBox Pure core to let players do retro-lan-partys over the RetroArch netplay feature.
This doesn't use the savestate syncronisation, but allows to send netpackets.
gpSP was the first core besides DosBoxPure to use this feature for the emulation of the GBA Wireless Adapter. 
Ninoh-Fox, the creator of the [koriko cfw](https://github.com/Rparadise-Team/Koriki) for the miyoo mini, asked me, if i could implement this feature into DoubleCherryGB to enable trading for GB/GBC and i did.

Why is this easier? No setup and no workarounds. No need to own the same roms (region, edition). Should work cross platform.
You can find trading partners over the netplay lobby (netplay -> refresh Host List), and can use the netplay text-chat by pressing ^ on your keyboard.
Also works in LAN. 

I recommend to use the [RetroArch Discord server](https://discord.com/channels/184109094070779904/326960443157381120) to find players and use voice-chats.

Atleast Retroarch 1.17 is needed for the netpacket api.
Only the linkcable data is transfered over network, so you won't see the others player's screen.

- make sure emulated gameboys is set to 1 in the Core Options 
- load your rom
- start or join a netplay (a second player can join any time)

The netpacket api is activated when you set emulated gameboys to 1. This is only meant for trading purposes and may be too slow for actual Multiplayer.
For Multiplayer-sessions please set the amount of emulated gameboys to 2 or higher, because this will activate the old savestate sync for lagfree netplay.  

Trading should work with other GB/GBC Games as well:

- Pokemon R/B/Y/S/G/C
- Pokemon Trading Card Game 1 and 2
- RoboPon Sun/Star
- Telefang Speed/Power
- Dragon Warrior (Quest) Monster
- Dragon Warrior (Quest) Monster 2 Tara's Adventure / Cobi's Journey
- Monster Rancher Battle Card Game
- Zelda OoA / OoS ring trade
- ...

Infrared over network is currently not supported (GBE+ does) but maybe in a future release.

Have fun!


If you like to support me
https://patreon.com/double_cherry_projects<br>
https://www.buymeacoffee.com/timoelrichs<br>

Thanks to
- Shonumi for doing the research and technical documentation for Gameboy and many peripherals/accessories/add-ons  https://shonumi.github.io/index.html
- PSYRaven for the netpacket api and davidgf for his implemtation on gpSP
- Ninoh-Fox for his work on Koriki cfw
- GIGO and Hii for making TGBDual and the libretro port

![](/screenshots/Faceball%202000.png)
![](/screenshots/Tetris.png)
![](/screenshots/tetris_winner.png)
![](/screenshots/Kwirk.png)

Original sources from
- https://github.com/libretro/tgbdual-libretro
- http://gigo.retrogames.com/download.html#tgb-dual
- http://shinh.skr.jp/tgbdualsdl/



