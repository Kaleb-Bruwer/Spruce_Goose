# Spruce Goose

This is a Minecraft server written from scratch in C++, that is compatible with Minecraft 1.7.9 clients. While this is not a complete implementation of the game, not even close, it does provide some of the core gameplay features.

## How to use

From a linux terminal, run the following commands to compile from source:
```
mkdir Build
cd Build
cmake ..
make
```

Then, the server can be launched with
```
./spruce_goose 25565
```
where 25565 is an optional argument to specify the port number to be used. The default is 25565.

Once the server is running, a 1.7.9 Minecraft client can be used to connect to it as with any other multiplayer server. If connecting from the same computer, the address will likely be ```localhost:25565```.

The build process also produces a ```test``` executable, which contains the project's unit tests.

## Features

1. Place/break blocks
2. Inventory system
3. Multiplayer
4. Crafting
5. Load/unload chunks as player(s) move
6. Compatability with minecraft's file format for terrain (.mca)

## Motivation

This project started out of a curiosity with physics/game engines, but quickly morphed into an opportunity to build experience in C++ and the development of (relatively) large computer programs. This project spanned a wide range of topics, including network communication, multithreading, the design of efficient and task-specific datastructures, the use of spatial data and software testing.

## Why the name?

This project didn't turn out to be useful for anything, but was quite interesting to work on none the less.