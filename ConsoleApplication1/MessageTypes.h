#pragma once

const int EClient_Hello = 0;         //
const int EClient_Name = 1;        // name
const int EClient_RequestRefresh = 2;//
const int EClient_RequestPlay = 3;   //
const int EClient_RequestMove = 4;   // position
const int EClient_RequestRematch = 5;//
const int EClient_RejectRematch = 6;


const int EServer_RequestName = 0;   //
const int EServer_Lobby = 1;         // player1,player2,player3;player4,player5
const int EServer_MatchStart = 2;    // yourturn, vsplayername
const int EServer_MatchUpdate = 3;   // yourturn, board
const int EServer_MatchWon = 4;      // board
const int EServer_MatchLost = 5;     // board
const int EServer_MatchDraw = 6;     // board
const int EServer_BadInput = 7;      // reason
