#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

class StartMenu : public QWidget {
    Q_OBJECT
public:
    StartMenu(QWidget *parent = nullptr);

signals:
    void newGameClicked();
    void continueGameClicked();
    void instructionsClicked();

private:
    QPushButton *newGameButton;
    QPushButton *continueGameButton;
    QPushButton *instructionsButton;
};
