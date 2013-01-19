This is the source code for an Android Pipboy 3000 simulator I threw together over a few weeks in 2011. I would have loved the chance to really tidy it up and make everything professional, but I have neither the time nor the motivation really so I'm just releasing as-is in case anybody can make use of it.

Usage
-----
The bulk of the source code is for the pipboy app, which has been helpfully compiled already in the bin directory so you can just install it on your phone. However, the app looks for a zip file full of resources from the game, which I'm legally not allowed to distribute. I've been working on a script to extract the relevant files from an install of Fallout 3 or Fallout New Vegas, but it's not ready yet, so right now you'll have to take a look at the note.txt in the zip directory to see which files need to be in it and try to construct it yourself.

 - Construct a resource zip file that looks like the one described in zip/note.txt
 - put it on your device at /storage/sdcard0/pipboy/data.zip
 - install the apk
 - Run it!

Control
-------

The primary form of control is meant to be from hardware buttons. However you implement your hardware, it should send TCP packets to the phone on port 4919 to control it. The exact format of the packets isn't documented yet, have a look at the arduino sketch or the source code for handling the packets on the phone side.

There is also a debug touchscreen interface. Swiping left and right switches between tabs, and directional navigation is by clicking in the top,left,right or bottom parts of the screen. Long press is select, though it can be tricky to make that take sometimes.

Known Bugs
----------
 - Sometimes it loads, but there's a black screen. Pressing home, then switching back to the app seems to fix it, but I have no idea why it happens and no time to fix it.
 - The loadout is hard-coded. I had every intention of fixing this and making it configurable via config file, but I haven't gotten around to it yet.

FAQ
---
 **Why did you code this in C++ rather than Java**

 Because I don't know Java at all. 


 **Looking at your C++ it doesn't exactly look like you know that language though**

 Shut up. One header file for everything is a great choice! Like I said, This was just hacked together over a few weekends

 **I saw the sketch, where's the circuit diagram?**

 Working on it!
