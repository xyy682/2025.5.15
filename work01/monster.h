#pragma once
#include <QGraphicsPixmapItem>

class Monster {
public:
    int x, y, hp, atk;
    int animFrame = 0;
    int faceDir = 1;
    bool isMoving = false;

    Monster(int x, int y) : x(x), y(y), hp(10), atk(3) {}

    void updateAnimation() {
        animFrame = (animFrame + 1) % 2; // 0-1循环
    }
};
