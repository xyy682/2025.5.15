#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    dungeonView = new DungeonView(this);
    setCentralWidget(dungeonView);
    setWindowTitle("地牢寻宝者 Dungeon Delver");
    resize(800, 600);
}

MainWindow::~MainWindow() {
    delete ui;
}
