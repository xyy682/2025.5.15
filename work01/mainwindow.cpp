#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dungeonView = new DungeonView(this);
    connect(dungeonView->getDungeon(), &Dungeon::playerStatusChanged, this, &MainWindow::updatePlayerStatus);
    connect(dungeonView->getDungeon(), &Dungeon::gameWin, this, &MainWindow::onGameWin);
    connect(dungeonView->getDungeon(), &Dungeon::gameOver, this, &MainWindow::onGameOver);
    // 新增：创建属性显示标签
    playerStatusLabel = new QLabel(this);
    playerStatusLabel->setText("HP: 20  ATK: 5  DEF: 2");
    playerStatusLabel->setAlignment(Qt::AlignLeft);
    playerStatusLabel->setFixedHeight(30);

    // 新增：用布局把标签和游戏视图都加到主窗口
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(playerStatusLabel);
    layout->addWidget(dungeonView);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setWindowTitle("地牢寻宝者 Dungeon Delver");
    resize(800, 600);

    updatePlayerStatus(); // 初始化时刷新一次

}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::updatePlayerStatus() {
    // 获取玩家对象
    Player* player = dungeonView->getDungeon()->getPlayer();
    int level = dungeonView->getDungeon()->getLevel();
    QString status = QString("Level: %1  HP: %2  ATK: %3  DEF: %4")
                         .arg(level)
                         .arg(player->hp)
                         .arg(player->atk)
                         .arg(player->def);
    playerStatusLabel->setText(status);}
void MainWindow::onGameWin() {
    int nextLevel = dungeonView->getDungeon()->getLevel() + 1;
    QMessageBox::information(this, "胜利", QString("你成功到达出口，进入第%1关！").arg(nextLevel));
    // 进入下一关：只重置地牢，不重置玩家属性
    Dungeon* oldDungeon = dungeonView->getDungeon();
    Player* player = oldDungeon->getPlayer();

    // 新建地图，关卡+1，保留上一把的属性啊啊1111
    Dungeon* newDungeon = new Dungeon();
    newDungeon->setLevel( nextLevel);

    newDungeon->getPlayer()->hp = player->hp;
    newDungeon->getPlayer()->atk = player->atk;
    newDungeon->getPlayer()->def = player->def;

    // 替换 dungeonView 的 dungeon 指针
    dungeonView->replaceDungeon(newDungeon);

    // 重新连接信号
    connect(newDungeon, &Dungeon::playerStatusChanged, this, &MainWindow::updatePlayerStatus);
    connect(newDungeon, &Dungeon::gameWin, this, &MainWindow::onGameWin);
    connect(newDungeon, &Dungeon::gameOver, this, &MainWindow::onGameOver);

    updatePlayerStatus();
}

void MainWindow::onGameOver() {
    QMessageBox::warning(this, "失败", "你被怪物打败了！\n游戏将重开。");
    // 重新生成地牢
    delete dungeonView;
    dungeonView = new DungeonView(this);
    // 重新布局
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(centralWidget()->layout());
    layout->addWidget(dungeonView);
    // 重新连接信号
    connect(dungeonView->getDungeon(), &Dungeon::playerStatusChanged, this, &MainWindow::updatePlayerStatus);
    connect(dungeonView->getDungeon(), &Dungeon::gameWin, this, &MainWindow::onGameWin);
    connect(dungeonView->getDungeon(), &Dungeon::gameOver, this, &MainWindow::onGameOver);
    updatePlayerStatus();
}
