#pragma once
#include <QGraphicsView>
#include <QKeyEvent>
#include "dungeon.h"

class DungeonView : public QGraphicsView {
    Q_OBJECT
public:
    DungeonView(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    Dungeon *dungeon;
};
