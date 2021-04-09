#pragma once
/*
game.h
*/

#include <SDL.h>
#include <stdbool.h>

struct game {
    SDL_Window* window;
    SDL_Renderer* rend;

    int mouseX, mouseY, mouseWheelX, mouseWheelY, mouseInitX, mouseInitY, width, height;
    bool running, up, down, left, right, ctrl, mouseLeftDown, mouseRightDown, mouseLeftUp, mouseRightUp, mouseDrag, mouseDragged;

    int scrolling; // flag (scrolling or not)
    int scroll_sensitivity; // how fast we want to scroll
    double scroll_Y; // current scrolling amount (on Y-Axis)
    double scroll_acceleration; // scrolling speed
    double scroll_friction; // how fast we decelerate
    double scroll_prev_pos; // previous event's position
};

struct game* g;

void Game_Init(char* windowName, int width, int height);
void Game_PollInput();
void Game_BeginDraw();
void Game_EndDraw();