Arda Land - Brady Ancell (Sir Byzler) | Matias Pena (Ploopy) | Garrison Cook (Marcel)
MP - Over Hill and Under Hill
A world with 3 heros (Sir Byzler, Ploopy, and Marcel).

USAGE: Run the executable after compiling. 
There are 2 camera modes, ArcBallCam and FreeCam. Toggle between them using Space. Default is ArcBallCam. 

In ArcBallCam, use WASD to control the hero, and click and drag mouse to rotate the camera. Click and drag mouse while holding shift to zoom in and out.

If in FreeCam mode, use W and S to move the camera forward and backward. Click and drag the mouse to rotate the camera.

You can also toggle a picture-in-picture first person mode using F.

To switch between heros (for ArcBallCam and first person camera), use < and >.

INSTRUCTIONS FOR COMPILING: Run `cmake CMakeLists.txt`. Then `make` which will create an executable named "mp". Run with `./mp`.

BUGS: the camera will teleport do a different position when you first try to rotate it using the mouse. Positioning and attenuation of spotlight and point spotlight 
are iffy. Players can drive off the grid without their camera following.

WORK DISTRIBUTION:
Brady - shaders/textures, implemented switching between heros, pointlight, skybox, byzler
Matias - first person cam, picture in picture, spotlight, ploopy
Garrison - world objects (buildings & trees), marcel

QUESITONS:
How long did this assignment take you? 
~10-15 hours
How much did the lab help you for this assignment?
5. 
How fun was this assignment?
6. Pretty fun but hard to implement complex we talked about weeks ago with few examples to go off of.
We had no labs that used both texture and solid material color at the same time, no labs with multiple light sources
or even different kinds.
