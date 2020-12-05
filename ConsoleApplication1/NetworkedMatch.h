#pragma once
#include "TicTacToeGame.h"
#include <iostream>
#include <sstream>
#include <WS2tcpip.h>
#include "DataStructures.h"
#include "MessageTypes.h"
class NetworkedMatch
{
public:
    int MatchId = -1;
    int player1Id = -1;
    int player2Id = -1;
    TicTac Game;
    NetworkedMatch(int player1, int player2, int matchId)
    {
        MatchId = matchId;
        player1Id = player1;
        player2Id = player2;
    }

    bool TryPlay(int playerId, byte position)
    {
        int player = 0;
        if (playerId == player1Id)
        {
            player = 1;
        }
        else if (playerId == player2Id)
        {
            player = 2;
        }
        else {
            return false;
        }

        return Game.TryPlay(player, position);
    }
};