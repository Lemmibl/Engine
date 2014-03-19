Engine
======

DirectX11 / C++ videogame "Engine" made by me, Carl "Lemmi" Engström. Originally started out as a Bachelor's Degree project together with my friend Felix Nawrin Oxing, with the focus on procedurally generated terrain and deferred rendering.

***********************************
IMPORTANT:

This project has really only been tested on Windows 7 64-bit with an nvidia card. I know for a fact that it runs worse on my laptop with an ATI 5650; for some reason there's loads of z-fighting and texture filtering is all messed up.

For a real installer to the project, here's the newest (last updated March 19, 2014): ftp://lemminas.dlinkddns.com/Public/LemmiInstaller.msi

It should work fine, as long as your x86 vc++ 2010 redist is up to date: (http://www.microsoft.com/en-us/download/details.aspx?id=5555)

and your directx redist is up to date: http://www.microsoft.com/en-us/download/details.aspx?id=8109

If you want to download the project as a zip and compile it and have several versions of visual studio, you need to start the solution file with a specific version of VS, as opposed to the VS version selector. Else it somehow can't decide which one to pick and just doesn't start.

***********************************


Here are the controls for this project. They are probably prone to change and I probably won't update this readme every time, but here goes:

* Escape to return to main menu/exit (if you want to exit just press ESC a few times)
* Arrow keys OR mouse to look around.
* W-A-S-D to move forward, left, backward and right respectively.
* Space to move upward.
* C to move downward.
* Hold left shift to move faster.
* Hold left control to move slower.
* T to toggle noclip

* Q to toggle drawing debug info.
* E to toggle wireframe rendering
* G to reload settings in realtime. Change something in settings.cfg and see for yourself.
* N to generate a new world.
* R and F to move point lights up and down.

* 1 to speed up the day/night cycle.
* 2 to toggle material drawing mode.
* 3 and 4 are supposed to change fog draw distance but is currently not supported...
* 5 to toggle drawing of point lights
* 6 to toggle SSAO drawing mode. (Default is with SSAO enabled, next is the scene without any SSAO, and the last is the scene with only SSAO applied).

***********************************
* F1-F9 to change what type of terrain you want to generate next.

* F1: TypesSeaBottom.
* F2: TypePlains.
* F3: TypeHills.
* F4: TypeTerraces.
* F5: TypeDramaticHills.
* F6: TypeFlyingIslands.
* F7: TypeAlien.
* F8: TypeFancy.
* F9: TypeCave.
***********************************
