#pragma once
#include <QGraphicsView>
#include <QKeyEvent>

#include "dungeon.h"

class DungeonView : public QGraphicsView {
    Q_OBJECT
public:
    Dungeon* getDungeon() { return dungeon; }
    DungeonView(QWidget *parent = nullptr);
    void replaceDungeon(Dungeon* newDungeon);
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    Dungeon *dungeon;
};
