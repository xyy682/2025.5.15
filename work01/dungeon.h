#pragma once
#include <QGraphicsScene>
#include <QVector>
#include "player.h"
#include "monster.h"
#include "item.h"

class Dungeon {
public:
    Dungeon(QObject *parent = nullptr);
    QGraphicsScene* scene() { return m_scene; }
    bool handlePlayerMove(int key);
private:
    void generateDungeon();
    void updateVisibility();
    QGraphicsScene *m_scene;
    QVector<QVector<int>> map; // 0:墙 1:地板 2:楼梯
    Player *player;
    QList<Monster*> monsters;
    QList<Item*> items;
    int width, height;
};
