# Step Platformio development environment

The aim of this file is to guide you for setting up a development instrument that is similar to the one I used when developing the code. In all the following, I assume Ubuntu 20.04LTS (more recent Ubuntu LTS will likely work in the same way). I strongly recommend you to use an Ubuntu LTS, if not, you are on your own!

## Installing visual studio code (VSC)

See the instructions at: https://code.visualstudio.com/docs/setup/linux . I think that using snap is the simplest solution, so I simply:

```
jr@T490:~$ sudo snap install --classic code
[sudo] password for jr: 
code ccbaa2d2 from Visual Studio Code (vscode✓) installed
```

Start VSC, let it "warm up" and update, possibly restart it a couple of times, and you should be ready to go with VSC.

## Installing various dependencies, VSC extensions including platformio

You will need a few "dependencies":

```
jr@T490:~$ sudo apt-get install python3-venv
jr@T490:~$ sudo apt-get install git
```

To have a good coding environment, I recommend using a few VSC extensions:

- Vim, from vscodevim,
- C/C++, from Microsoft,
- C/C++ Extension Pack, from Microsoft,
- Bracket Pair Colorizer 2, from CoenraadS,
- Code Spell Checker, from Street Side Software,
- Blockman - Highlight Nested Code Blocks, from leodevbro,
- Highlight Matching TAg, from vincaslt,
- PlatformIO IDE, from PlatformIO.

## Installing the Sparkfun Ambiq Apollo3 core v1 platformio core

The Sparkfun core is made available to PlatformIO through a community-contributed core, available at: https://github.com/nigelb/platform-apollo3blue . In particular, we will follow the instructions at https://github.com/nigelb/platform-apollo3blue/blob/master/docs/platform-apollo3blue/UsingMultipleVersionsOfArduino_Apollo3.md , section Install framework-arduinoapollo3@1.2.3 .

To install in the right version, assuming a "standard" Ubuntu setup, home, paths etc (some of the following commands will take quite some time, the toolchains to download are quite large!):

```
jr@T490:~$ cd .platformio/
jr@T490:~/.platformio$ cd packages/
jr@T490:~/.platformio/packages$ git clone --recurse-submodules --branch v1.2.3 https://github.com/sparkfun/Arduino_Apollo3.git framework-arduinoapollo3@1.2.3
Cloning into 'framework-arduinoapollo3@1.2.3'...
remote: Enumerating objects: 12135, done.
remote: Total 12135 (delta 0), reused 0 (delta 0), pack-reused 12135
Receiving objects: 100% (12135/12135), 2.80 GiB | 6.10 MiB/s, done.
Resolving deltas: 100% (7291/7291), done.
Note: switching to '87a33ce12049ff10c6d0d7e6ab9038e4d874b36e'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by switching back to a branch.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -c with the switch command. Example:

  git switch -c <new-branch-name>

Or undo this operation with:

  git switch -

Turn off this advice by setting config variable advice.detachedHead to false
```

Then fix the ```package.json```, which has a typo in it, so that it reads:

```
jr@T490:~/.platformio/packages/framework-arduinoapollo3@1.2.3$ cat package.json 
{
    "name": "framework-arduinoapollo3",
    "description": "Arduino Wiring-based Framework (Apollo3 Core)",
    "version": "1.2.3",
    "url": "https://github.com/sparkfun/Arduino_Apollo3"
}
```

Finally, install the platformio core:

```
jr@T490:~$ cd .platformio/
jr@T490:~/.platformio$ ls
appstate.json  homestate.json  packages  penv
jr@T490:~/.platformio$ mkdir platforms
jr@T490:~/.platformio$ cd platforms/
jr@T490:~/.platformio/platforms$ git clone https://github.com/nigelb/platform-apollo3blue.git apollo3blue
Cloning into 'apollo3blue'...
remote: Enumerating objects: 724, done.
remote: Counting objects: 100% (724/724), done.
remote: Compressing objects: 100% (395/395), done.
remote: Total 724 (delta 394), reused 575 (delta 289), pack-reused 0
Receiving objects: 100% (724/724), 485.93 KiB | 3.13 MiB/s, done.
Resolving deltas: 100% (394/394), done.
```

At this stage, you should be ready to use the Sparkfun Apollo3 Ambiq core. To check it:

- open VSC
- open platformio: ```ctrl-shift-p``` > "PlatformIO Home"
- select "new project" > name: "test_apollo3", board: "SparkFun RedBoard Artemis", Framework: "Arduino", and "finish"

This should create a new project populated with a number of files; to be on the safe side, edit the ```platformio.ini``` file so that it reads:

```
[env:SparkFun_RedBoard_Artemis]
platform = apollo3blue
board = SparkFun_RedBoard_Artemis
framework = arduino
platform_packages = framework-arduinoapollo3@1.2.3
```

Now, going to the ```src/main.cpp``` file, you should be able to use the "compile" button at the bottom of VSC to compile the (default empty) sketch, and get a SUCCESS message. If this is the case, congratulations, you are ready to code! If not, check if / what you did wrong, or ask for help in the issues tracker.
