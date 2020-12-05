#pragma once

const int EClient_Hello = 0;         //
const int EClient_Name = 1;          // name
const int EClient_RequestMove = 2;   // position
const int EClient_RequestRematch = 3;//
const int EClient_RejectRematch = 4; //


const int EServer_RequestName = 0;   //
const int EServer_MatchStart = 1;    // vsplayername
const int EServer_MatchUpdate = 2;   // yourturn, board
const int EServer_MatchWon = 3;      // board
const int EServer_MatchLost = 4;     // board
const int EServer_MatchDraw = 5;     // board
