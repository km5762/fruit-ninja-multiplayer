Project Structure:

Three folders have been created for this project. The first, client, contains all client specific game objects and the Client.cpp object itself, which handles the connection to the server. The second, server, holds all server specific game objects and the Server.cpp object which manages connections to clients and synchronizing the game world. Finally, shared contains all game objects which are shared between client and server, and have been marked up with preprocessing instructions (#ifdef CLIENT) to compile each version. All object files compile to a obj/ directory in client/ and server/.

Compilation Instructions:

The project was built on Linux Mint, so all special compilation instructions for Linux apply. This means that, in the Makefiles, the dragonfly and SFML paths must be configured appropriately. Additionally, SFML must be added to LD_LIBRARY_PATH or the game will not run. After that, running make in both the client and server directories will build all object files.

Run Instructions:

First, the server must be run by running the server executable in /server. Then, the client can be run by running the fruit-ninja executable in /client. A prompt will allow clients to enter the hostname (127.0.0.1 for testing purposes), and will simply clear if an invalid address is entered. After 3 players connect, the server will start the game.

Other Notes:

The game window has been configured to be really small to allow for testing. Also note that the networked version of the game engine must be used, and the df-config.txt also accounts for this.

The game is built to start when 3 players connect, if someone tries joining mid game they will not be able to connect.

Clients can also disconnect mid game by pressing q. This is shown in the video.
