#pragma once
#include <QGraphicsScene>
#include <QVector>
#include <QList>
#include <QTimer>
#include <QGraphicsPolygonItem>
#include <QObject>
#include "player.h"
#include "monster.h"
#include "item.h"
struct Shockwave {
    int x, y; // 当前格子
    int dx, dy; // 方向
    QGraphicsPolygonItem* item;
    bool active;
};

class Dungeon  : public QObject{
    Q_OBJECT
public:
    QVector<QVector<int>> wallHitCount; // 记录每个格子的受击次数
    int lastMoveDx = 1; // 最后一次位移方向决定冲击波方向，，，默认向右
    int lastMoveDy = 0;
    void moveMonsters();//怪物移动函数
    Dungeon(QObject *parent = nullptr);
    QGraphicsScene* scene() { return m_scene; }
    bool handlePlayerMove(int key);
    bool isReachable(int sx, int sy, int ex, int ey);
    bool playerShockwave();
    void fireShockwave();
    Player* getPlayer() { return player; }
    void fireShockwave(int dx, int dy); // 单方向,函数重载这个是有参调用
    void fireShockwaveAll();          // 四方向  暂时舍弃
    void updateShockwaves();
    int getLevel() const { return level; }
    void setLevel(int l) { level = l; }
    bool hasKey() const { return m_hasKey; } // 获取钥匙状态
signals:
    void playerStatusChanged();
    void gameWin();
    void gameOver();
private:
    int level = 1; // 关卡数
    void generateDungeon();
    void updateVisibility();
    QGraphicsScene *m_scene;
    QVector<QVector<int>> map; // 0:墙 1:地板 2:楼梯
    Player *player;
    QList<Monster*> monsters;
    QList<Item*> items;
    int width, height;
    QList<Shockwave> shockwaves;
    QTimer* shockwaveTimer;
    bool m_hasKey = false; // 添加钥匙状态变量
};
