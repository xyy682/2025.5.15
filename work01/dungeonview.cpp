#include "dungeonview.h"
#include <QGraphicsScene>
#include <QMessageBox>

DungeonView::DungeonView(QWidget *parent)
    : QGraphicsView(parent) {
    dungeon = new Dungeon(this);
    setScene(dungeon->scene());
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setRenderHint(QPainter::Antialiasing);
}

void DungeonView::keyPressEvent(QKeyEvent *event) {
    if (dungeon->handlePlayerMove(event->key())) {
        scene()->update();
    }
}
