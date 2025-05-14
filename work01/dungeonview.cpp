#include "dungeonview.h"
#include <QGraphicsScene>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QKeyEvent>

DungeonView::DungeonView(QWidget *parent)
    : QGraphicsView(parent) {
    dungeon = new Dungeon;
    setScene(dungeon->scene());
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setRenderHint(QPainter::Antialiasing);
}
void DungeonView::keyPressEvent(QKeyEvent *event) {
    bool updated = false;
    if (event->isAutoRepeat()) return;
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_S || event->key() == Qt::Key_D || event->key() == Qt::Key_W) {
        updated = dungeon->handlePlayerMove(event->key());
    } else if (event->key() == Qt::Key_J) {
        dungeon->fireShockwave(); // 直接发射，不用计时
    }
    if (updated) scene()->update();
}
void DungeonView::replaceDungeon(Dungeon* newDungeon) {
    delete dungeon;
    dungeon = newDungeon;
    setScene(dungeon->scene());
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}
