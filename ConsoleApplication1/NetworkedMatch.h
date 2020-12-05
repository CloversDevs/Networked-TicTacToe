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
    int player1WantsRematch = 0;
    int player2WantsRematch = 0;
    int player1Id = -1;
    int player2Id = -1;
    TicTac Game;
    NetworkedMatch(int player1, int player2, int matchId)
    {
        MatchId = matchId;
        player1Id = player1;
        player2Id = player2;
    }
    int GetPlayerTeamFromId(int playerId)
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
        return player;
    }
    bool TryPlay(int playerId, byte position)
    {
        int player = GetPlayerTeamFromId(playerId);
        if(player == 0)
        {
            return false;
        }

        return Game.TryPlay(player, position);
    }

    bool BothPlayersWantRematch()
    {
        return player1WantsRematch == 1 && player2WantsRematch == 1;
    }

    bool APlayerWantsARematch()
    {
        return player1WantsRematch == 1 || player2WantsRematch == 1;
    }

    bool APlayerRejectedRematch()
    {
        return player1WantsRematch == -1 || player2WantsRematch == -1;
    }

    void PlayerRequestsRematch(int playerId)
    {
        int player = GetPlayerTeamFromId(playerId);
        if (player == 1)
        {
            player1WantsRematch = 1;
        }
        else if (player == 2)
        {
            player2WantsRematch = 1;
        }
    }

    void Reset()
    {
        player1WantsRematch = 0;
        player2WantsRematch = 0;
        Game = TicTac();
    }
};