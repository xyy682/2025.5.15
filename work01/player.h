#pragma once

class Player {
public:
    int x, y, hp, atk, def;
    Player(int x, int y) : x(x), y(y), hp(20), atk(5), def(2) {}
};
