#include "dungeon.h"
#include <QRandomGenerator>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>

Dungeon::Dungeon(QObject *parent) {
    width = 20; height = 15;
    m_scene = new QGraphicsScene(0, 0, width*32, height*32, parent);
    generateDungeon();
    updateVisibility();
}

void Dungeon::generateDungeon() {
    // 简单随机地牢生成
    map = QVector<QVector<int>>(height, QVector<int>(width, 0));
    for (int y=1; y<height-1; ++y)
        for (int x=1; x<width-1; ++x)
            map[y][x] = (QRandomGenerator::global()->bounded(100) < 70) ? 1 : 0;
    // 放置楼梯
    map[height-2][width-2] = 2;
    // 玩家
    player = new Player(1, 1);
    // 怪物
    monsters.clear();
    monsters.append(new Monster(5, 5));
    // 物品
    items.clear();
    items.append(new Item(3, 3));
}

void Dungeon::updateVisibility() {
    m_scene->clear();
    for (int y=0; y<height; ++y) {
        for (int x=0; x<width; ++x) {
            QGraphicsRectItem *cell = m_scene->addRect(x*32, y*32, 32, 32,
                                                       QPen(Qt::black), QBrush(map[y][x]==0 ? Qt::darkGray : Qt::lightGray));
            if (map[y][x]==2)
                m_scene->addRect(x*32+8, y*32+8, 16, 16, QPen(Qt::NoPen), QBrush(Qt::yellow));
        }
    }
    // 玩家
    m_scene->addEllipse(player->x*32+4, player->y*32+4, 24, 24, QPen(Qt::blue), QBrush(Qt::blue));
    // 怪物
    for (auto m : monsters)
        m_scene->addEllipse(m->x*32+8, m->y*32+8, 16, 16, QPen(Qt::red), QBrush(Qt::red));
    // 物品
    for (auto i : items)
        m_scene->addRect(i->x*32+12, i->y*32+12, 8, 8, QPen(Qt::green), QBrush(Qt::green));
}

bool Dungeon::handlePlayerMove(int key) {
    int dx=0, dy=0;
    if (key==Qt::Key_Up) dy=-1;
    else if (key==Qt::Key_Down) dy=1;
    else if (key==Qt::Key_Left) dx=-1;
    else if (key==Qt::Key_Right) dx=1;
    else return false;
    int nx = player->x + dx, ny = player->y + dy;
    if (nx<0 || ny<0 || nx>=width || ny>=height) return false;
    if (map[ny][nx]==0) return false;
    // 检查怪物
    for (auto m : monsters) {
        if (m->x==nx && m->y==ny) {
            m->hp -= player->atk;
            if (m->hp<=0) monsters.removeOne(m);
            updateVisibility();
            return true;
        }
    }
    // 检查物品
    for (auto i : items) {
        if (i->x==nx && i->y==ny) {
            player->hp += 5; // 假设是药水
            items.removeOne(i);
            break;
        }
    }
    player->x = nx; player->y = ny;
    updateVisibility();
    return true;
}
