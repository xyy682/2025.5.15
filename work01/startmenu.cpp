#include "startmenu.h"

StartMenu::StartMenu(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    newGameButton = new QPushButton("新的游戏", this);
    continueGameButton = new QPushButton("继续游戏", this);
    instructionsButton = new QPushButton("操作说明", this);

    layout->addWidget(newGameButton);
    layout->addWidget(continueGameButton);
    layout->addWidget(instructionsButton);

    connect(newGameButton, &QPushButton::clicked, this, &StartMenu::newGameClicked);
    connect(continueGameButton, &QPushButton::clicked, this, &StartMenu::continueGameClicked);
    connect(instructionsButton, &QPushButton::clicked, this, &StartMenu::instructionsClicked);
}
