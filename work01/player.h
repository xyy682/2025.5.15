#pragma once
#include <QGraphicsPixmapItem>

class Player {
public:
    int x, y, hp, atk, def;
    bool hasKey;
    int animFrame = 0;
    int faceDir = 1; // 1=right, -1=left
    bool isMoving = false;

    Player(int x, int y) : x(x), y(y), hp(20), atk(5), def(2), hasKey(false) {}

    void updateAnimation() {
        if (isMoving) {
            animFrame = (animFrame + 1) % 3; // 循环0-1-2
        } else {
            animFrame = 3 + (animFrame % 2); // 循环3-4
        }
    }
};
