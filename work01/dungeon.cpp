#include "dungeon.h"
#include <QRandomGenerator>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <queue>
#include <QTimer>
Dungeon::Dungeon(QObject *parent ) {
    width = 20; height = 15;
    m_scene = new QGraphicsScene(0, 0, width*32, height*32, parent);
    generateDungeon();
    updateVisibility();
    shockwaveTimer = new QTimer(this);
    connect(shockwaveTimer, &QTimer::timeout, this, [this]() { updateShockwaves(); });
}

void Dungeon::generateDungeon() {
    int exitX, exitY;
    do {
        // 1. 随机生成地图
        map = QVector<QVector<int>>(height, QVector<int>(width, 0));
        for (int y=1; y<height-1; ++y)
            for (int x=1; x<width-1; ++x)
                map[y][x] = (QRandomGenerator::global()->bounded(100) < 70) ? 1 : 0;

        // 2. 玩家起点
        player = new Player(1, 1);
        map[player->y][player->x] = 1; // 保证起点是地板

        // 3. 出口（楼梯），随机选择一个地板格子，且不与玩家重叠
        do {
            // 只在右半边生成出口
            exitX = QRandomGenerator::global()->bounded(width / 2, width - 1);
            exitY = QRandomGenerator::global()->bounded(1, height - 1);
        } while ((exitX == player->x && exitY == player->y) || map[exitY][exitX] != 1);
        map[exitY][exitX] = 2;

        // 4. 检查连通性（玩家到出口）
    } while (!isReachable(player->x, player->y, exitX, exitY));

    // 5. 怪物随机生成，数量和攻击力随关卡提升
    monsters.clear();
    int monsterCount = QRandomGenerator::global()->bounded(3 + level, 7 + level); // 随关卡增加
    for (int i = 0; i < monsterCount; ++i) {
        int mx, my, tryCount = 0;
        while (true) {
            mx = QRandomGenerator::global()->bounded(1, width-1);
            my = QRandomGenerator::global()->bounded(1, height-1);
            ++tryCount;
            bool overlap = false;
            if ((mx == player->x && my == player->y) || map[my][mx] != 1 || (mx == exitX && my == exitY))
                overlap = true;
            for (auto m : monsters) {
                if (m->x == mx && m->y == my) {
                    overlap = true;
                    break;
                }
            }
            if (!overlap || tryCount >= 100) break;
        }
        if (tryCount < 100) {
            Monster* monster = new Monster(mx, my);
            monster->atk += level; // 怪物攻击力随关卡提升
            monsters.append(monster);
        }
    }

    // 6. 物品生成（可选，和怪物类似）
    items.clear();
    int itemCount = QRandomGenerator::global()->bounded(1, 4);
    bool hasKey = false; // 标记是否已经生成了钥匙

    for (int i = 0; i < itemCount; ++i) {
        int ix, iy, tryCount = 0;
        while (true) {
            ix = QRandomGenerator::global()->bounded(1, width-1);
            iy = QRandomGenerator::global()->bounded(1, height-1);
            ++tryCount;
            bool overlap = false;
            if ((ix == player->x && iy == player->y) || map[iy][ix] != 1 || (ix == exitX && iy == exitY))
                overlap = true;
            for (auto m : monsters) {
                if (m->x == ix && m->y == iy) {
                    overlap = true;
                    break;
                }
            }
            for (auto it : items) {
                if (it->x == ix && it->y == iy) {
                    overlap = true;
                    break;
                }
            }
            if (!overlap || tryCount >= 100) break;
        }
        if (tryCount < 100) {
            ItemType type;
            if (!hasKey) {
                type = Key; // 确保至少生成一把钥匙
                hasKey = true;
            } else {
                type = static_cast<ItemType>(QRandomGenerator::global()->bounded(0, 3));
            }
            items.append(new Item(ix, iy, type));
        }
    }

    m_hasKey = false; // 重置钥匙状态
    // 初始化墙壁受击次数
    wallHitCount = QVector<QVector<int>>(height, QVector<int>(width, 0));
}

void Dungeon::updateVisibility() {
    // 只移除非冲击波的item
    QList<QGraphicsItem*> itemsToRemove;
    for (auto item : m_scene->items()) {
        // 不是冲击波的item才移除
        if (!item->data(0).toBool()) {
            itemsToRemove.append(item);
        }
    }
    for (auto item : itemsToRemove) {
        m_scene->removeItem(item);
        delete item;
    }

    // 重新绘制地牢、角色、怪物、物品
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
    for (auto i : items) {
        QColor color;
        switch (i->type) {
        case Heal: color = Qt::green; break;
        case AttackUp: color = Qt::red; break;
        case DefenseUp: color = Qt::blue; break;
        case Key: color = Qt::yellow; break; // 钥匙显示为黄色
        }
        m_scene->addEllipse(i->x*32+12, i->y*32+12, 8, 8, QPen(color), QBrush(color));
    }

        emit playerStatusChanged();

}
bool Dungeon::handlePlayerMove(int key) {
    int dx=0, dy=0;
    if (key==Qt::Key_W) dy=-1;
    else if (key==Qt::Key_S) dy=1;
    else if (key==Qt::Key_A) dx=-1;
    else if (key==Qt::Key_D) dx=1;
    else return false;
    int nx = player->x + dx, ny = player->y + dy;
    if (nx<0 || ny<0 || nx>=width || ny>=height) return false;
    if (map[ny][nx]==0) return false;
    lastMoveDx = dx;
    lastMoveDy = dy;//记录下最后一次位移的方向，也就是冲击波方向
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
        if (i->x == nx && i->y == ny) {
            switch (i->type) {
            case Heal:
                player->hp += 10;
                break;
            case AttackUp:
                player->atk += 2;
                break;
            case DefenseUp:
                player->def += 2;
                break;
            case Key:
                m_hasKey = true; // 获得钥匙
                break;
            }
            items.removeOne(i);
            break;
        }
    }

    player->x = nx; player->y = ny;

    // 检查是否到达出口
    if (map[ny][nx] == 2) {
        if (!m_hasKey) {
            // 如果没有钥匙，退回原位
            player->x = nx - dx;
            player->y = ny - dy;
            return true;
        }
        emit gameWin();
        return true;
    }

moveMonsters();
    // 玩家移动后，怪物攻击玩家（四周怪物都攻击一次）
    int px = player->x, py = player->y;
    int dxs[4] = {1, -1, 0, 0}, dys[4] = {0, 0, 1, -1};
    for (int d = 0; d < 4; ++d) {
        int mx = px + dxs[d], my = py + dys[d];
        for (auto m : monsters) {
            if (m->x == mx && m->y == my) {
                player->hp -= m->atk;
            }
        }
    }
    if (player->hp <= 0) {
        emit gameOver();
        return true;
    }
                         //
    updateVisibility();
    emit playerStatusChanged();
    return true;
}




bool Dungeon::isReachable(int sx, int sy, int ex, int ey) {
    QVector<QVector<bool>> visited(height, QVector<bool>(width, false));
    std::queue<std::pair<int, int>> q;
    q.push({sx, sy});
    visited[sy][sx] = true;
    int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        if (x == ex && y == ey) return true;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                !visited[ny][nx] && map[ny][nx] != 0) {
                visited[ny][nx] = true;
                q.push({nx, ny});
            }
        }
    }
    return false;
}
bool Dungeon::playerShockwave() {
    // 四个方向一格内的怪物全部受击
    bool hit = false;
    int px = player->x, py = player->y;
    int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    QList<Monster*> toRemove;
    for (int d = 0; d < 4; ++d) {
        int tx = px + dx[d], ty = py + dy[d];
        for (auto m : monsters) {
            if (m->x == tx && m->y == ty) {
                toRemove.append(m);
                hit = true;
            }
        }
    }
    for (auto m : toRemove) {
        monsters.removeOne(m);
    }
    if (hit) updateVisibility();
    return hit;
}
void Dungeon::fireShockwave(int dx, int dy) {
    if (dx == 0 && dy == 0) return;
    int px = player->x, py = player->y;
    int x = px + dx, y = py + dy;
    if (x < 0 || x >= width || y < 0 || y >= height || map[y][x] == 0) return; // 起点就是墙/边界
    QPolygonF diamond;
    float cx = x*32+16, cy = y*32+16;
    diamond << QPointF(cx, cy-8) << QPointF(cx+8, cy) << QPointF(cx, cy+8) << QPointF(cx-8, cy);
    QGraphicsPolygonItem* item = m_scene->addPolygon(diamond, QPen(Qt::blue), QBrush(Qt::blue));
    item->setData(0, true);
    item->setZValue(10);
    Shockwave sw;
    sw.x = x; sw.y = y; sw.dx = dx; sw.dy = dy; sw.item = item; sw.active = true;
    shockwaves.append(sw);
    if (!shockwaveTimer->isActive())
        shockwaveTimer->start(50); // 每50ms推进一格
}
void Dungeon::fireShockwave() {
    qDebug() << "fireShockwave() called";
    fireShockwave(lastMoveDx, lastMoveDy);
}
void Dungeon::fireShockwaveAll() {
     qDebug() << "fireShockwaveAll called!";
    fireShockwave(1, 0);
    fireShockwave(-1, 0);
    fireShockwave(0, 1);
    fireShockwave(0, -1);
}
void Dungeon::updateShockwaves() {
    QList<Shockwave> stillActive;
    for (auto& sw : shockwaves) {
        if (!sw.active) continue;
        int nx = sw.x + sw.dx, ny = sw.y + sw.dy;
        if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
            m_scene->removeItem(sw.item);
            delete sw.item;
            continue;
        }
        if (map[ny][nx] == 0) { // 是墙
            wallHitCount[ny][nx]++;
            if (wallHitCount[ny][nx] >= 3) {
                map[ny][nx] = 1; // 变成地板
                wallHitCount[ny][nx] = 0; // 重置
                updateVisibility(); // 重新绘制
            }
            m_scene->removeItem(sw.item);
            delete sw.item;
            continue;
        }
        // 检查怪物
        bool hitMonster = false;
        for (auto m : monsters) {
            if (m->x == nx && m->y == ny) {
                monsters.removeOne(m);
                hitMonster = true;
                break;
            }
        }
        if (hitMonster) {
            m_scene->removeItem(sw.item);
            delete sw.item;
            continue;
        }
        // 正常推进
        sw.x = nx; sw.y = ny;
        float cx = nx*32+16, cy = ny*32+16;
        QPolygonF diamond;
        diamond << QPointF(cx, cy-8) << QPointF(cx+8, cy) << QPointF(cx, cy+8) << QPointF(cx-8, cy);
        sw.item->setPolygon(diamond);
        stillActive.append(sw);
    }
    shockwaves = stillActive;
    updateVisibility();
    if (shockwaves.isEmpty())
        shockwaveTimer->stop();
}
void Dungeon::moveMonsters() {
    for (auto m : monsters) {
        // 计算怪物到玩家的距离
        int dx = player->x - m->x;
        int dy = player->y - m->y;
        int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
        int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

        // 怪物优先横向移动，如果横向不能走再尝试纵向
        bool moved = false;
        // 检查目标格子是否可走（地板、没有怪物、不是玩家、不是出口）
        int tx = m->x + stepX, ty = m->y;
        bool canMoveX = (stepX != 0) && (tx >= 0 && tx < width && ty >= 0 && ty < height)
                        && map[ty][tx] == 1
                        && !(player->x == tx && player->y == ty)
                        && std::none_of(monsters.begin(), monsters.end(), [tx, ty, m](Monster* other){ return other != m && other->x == tx && other->y == ty; });

        if (canMoveX) {
            m->x = tx;
            moved = true;
        } else {
            tx = m->x; ty = m->y + stepY;
            bool canMoveY = (stepY != 0) && (tx >= 0 && tx < width && ty >= 0 && ty < height)
                            && map[ty][tx] == 1
                            && !(player->x == tx && player->y == ty)
                            && std::none_of(monsters.begin(), monsters.end(), [tx, ty, m](Monster* other){ return other != m && other->x == tx && other->y == ty; });
            if (canMoveY) {
                m->y = ty;
                moved = true;
            }
        }
        // 如果不能移动就原地不动
    }
}
