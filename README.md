# Spruce Goose

This is a Minecraft server written from scratch in C++, that is compatible with Minecraft 1.7.9 clients. While this is not a complete implementation of the game, not even close, it does provide some of the core gameplay features.

## How to use

It is recommended to build the project using docker:
```
sudo docker build . -t spruce_goose/spruce_goose
```
When you run the container, you need to map port 25565 and directory sg/Save:
```
sudo docker run -v ./Save:/sg/Save -p 25565:25565 spruce_goose/spruce_goose
```

Once the server is running, a 1.7.9 Minecraft client can be used to connect to it as with any other multiplayer server. If connecting from the same computer, the address will be ```localhost:[port number]```.

The build process also produces a ```test``` executable, which contains the project's unit tests.

## Features

1. Place/break blocks
2. Inventory system
3. Multiplayer
4. Crafting
5. Load/unload chunks as player(s) move
6. Compatability with minecraft's file format for terrain (.mca)
7. Random segfaults

## Motivation

I started this project due to a curiosity for physics/game engines, but it quickly morphed into an opportunity to build experience in C++ as my first (relatively) large development. This project spanned a wide range of topics, including TCP network communication, multithreading, the design of efficient and task-specific datastructures, the use of spatial data and software testing.

## Why the name?

This project didn't turn out to be useful for anything, but was quite interesting to work on none the less.