#pragma once
#include <QMainWindow>
#include "dungeonview.h"
#include <QLabel>
#include <QVBoxLayout>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    DungeonView *dungeonView;
    QLabel* playerStatusLabel; // 新增
    void updatePlayerStatus();
private slots:
    void onGameWin();
    void onGameOver();
};
