This is the README for the Nano VM releases.
The Android release is version 3.4.3-2, the Linux and Windows versions are 3.4.3

The installation instructions are following.



ANDROID
=======

This is the experimental port of version 3.4.3-2 the Nano VM to Android ARM (ARM V7).

INSTALLATION
============

Note: The Nano directory should be copied on the sdcard:

/sdcard1/
    nanovm/
        prog/
        include/
        home/
        fonts/
    
The created or opened files always are in the home directory!
You can't write to a different path!

You have to install a terminal app now. I installed the Jack Palevich terminal app.
It's on Google play.

And install BusyBox.

Now copy the Nano binaries into the app directory of BusyBox: 
/data/data/burrows.apps.busybox/app_busybox

You can use the ADB shell to copy the files (available in the Android SDK platform tools).
Or copy the binaries to another location from where they can be executed.

Modify the home/startmenu.txt file, it's the full path to the Nano VM binary in the first line:
startmenu.txt:

/data/data/burrows.apps.busybox/app_busybox/nanovm
# program names for start menu:
tic-tac-toe-1.4
fractalix


Now you are ready to install the flow.apk file with the Flow GUI/graphics server.

I looked for a more comfortable way to install Nano, but this is the way it works.
Sorry for that.



USAGE
=====

You can run the compiler and assembler on Android, just like on Linux or Windows!


I included the "tic tac toe" game and fractalix (a fractal render program) as a demo:

1. Start the Flow app.

2. Click on the "tic-tac-toe-1.4" button.

3. Play the game.



NEW
===
ZERO INSTALL: NANOVM PORTABLE
=============================

Just copy the nanovm directory to the location where you want it to be.
For example an USB stick.

To run the programs just go into the directory where the binaries for your OS are.
On Linux this is:

nanovm/bin/linux-x86-64/

And then do:

$ sh ./runnanovm hello

To run program hello.


On Windows go into this directory:

nanovm/bin/windows-x86-64/

And do:

$ portnanovm hello


Note: all needed shared libraries should be included to run the executables.



LINUX
=====

INSTALLATION
============

This is a Nano VM binary release for Linux x86_64!

1. Copy the files from the bin directory to the bin directory in your /home directory.
All the needed shared libs are there also. You have to rename the files!
Just remove the "port" from the beginning:

flow
nanovm
nanoc
nanoa

2. Copy the nanovm directory into your /home directory.

3. Insert the followig line into your .bashrc bash config:

export NANOVM_ROOT=~/nanovm


USAGE
=====

Open a shell.
To compile hello.nc:

nanoc hello
nanoa hello


To run hello.no:

nanovm hello



Run flow (GUI) programs:

flow -menu



WINDOWS
=======

INSTALLATION
============

This is a Nano VM binary release for Windows!
Tested with Windows 10 64 Bit!

Copy this directory to C:\nanovm.


USAGE
=====

Open a shell.
To compile hello.nc:

nanoc hello
nanoa hello


To run hello.no:

nanovm hello



Run flow (GUI) programs:

flow -menu


------------------------

Have some fun!

Stefan Pietzonke (jay-t@gmx.net)   Saturday 13 August 2016
