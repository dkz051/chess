# Intranet Chess Duel

___Long assignment 2 for THU Programming Practices course, August 2019___

## Description
An intranet chess duel client/server.

## Environment
__Required__: Qt (5.13.0 or higher)

## Supported Rules
* __Move__
* __Capture__
* __Castle__: See _Unsupported Rules_ below for details
* __Promote__: for Pawns reaching the enemy's baseline
* __Checkmate__
* __Stalemate__
* __Proposed Draw__
* __Resign__
* __Timeout__: 1 minute for each move

## Unsupported Rules
* __En Passant__
* __Illegal Moves__: Illegal moves (a.k.a. letting your opponent capture your King while not checkmated) are not prohibited in this game.
* __Castle__: Castling is not prohibited for moved-and-returned King/Rooks in this game. You may even castle more than once during a single game.
* __Fifty-move Rule__
* __Threefold Repetition__
