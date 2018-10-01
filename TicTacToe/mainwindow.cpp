#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            field[i][j] = 'N';
        }
    }

    connect(ui->btnNewGame, &QPushButton::clicked, this,  &MainWindow::newGame);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {

        QPushButton *button = new QPushButton(this);

        connect(button, &QPushButton::clicked, [button, this, i, j](){
            char play = this->play(isPlayerTwoTurn);
            field[i][j] = play;

            button->setText(QString(play));

            isPlayerTwoTurn = !this->isPlayerTwoTurn;

            ui-> labelHasWinner->setText(isPlayerTwoTurn ? "Vez do X" : "Vez do O");

            button->setEnabled(false);

            if(this-> hasWinner(field, play)) {
                ui->labelHasWinner->setText(QString(play) + " ganhou");
            }
            else if (this->isDraw(field)) {
                ui->labelHasWinner->setText("Draw");
            }
        });
        ui->gridLayout->addWidget(button, i, j);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

char MainWindow::play(bool isPlayerTwoTurn) {
    if (isPlayerTwoTurn) return 'X';
    return 'O';
}

bool MainWindow::hasWinner(char newField[3][3], char state) {
    return hasRowColumnWinner(newField, state)
            || hasPrimaryDiagonalWinner(newField, state)
            || hasSecondaryDiagonalWinner(newField, state);
}

bool MainWindow::hasRowColumnWinner(char newField[3][3], char state) {
    int countStates = 0;
    // verificar por linha e coluna
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(newField[i][j] == state) countStates++;
        }
        if(countStates == 3) return true;
        countStates = 0;
    }
    return false;
}

bool MainWindow::hasPrimaryDiagonalWinner(char newField[3][3], char state) {
    int countStates = 0;

    // verificar na diagonal principal
    for(int i = 0, j = 0; i < 3; i++, j++) {
        if(newField[i][j] == state) countStates++;
    }
    if(countStates == 3) return true;

    return false;
}
bool MainWindow::hasSecondaryDiagonalWinner(char newField[3][3], char state) {
    int countStates = 0;

    // verificar na diagional secundárias
    for (int i = 2, j = 2; i >= 0; i--, j--) {
        if (newField[i][j] == state) countStates++;
    }
    if(countStates == 3) return true;

    // retorna falso se nenhuma for verdadeira
    return false;
}

bool MainWindow::isDraw(char newField[3][3]) {
    for(int i = 0; i < 3; i++) {
        for(int j =0; j < 3; j++) {
            if(newField[i][j] == 'N') return false;
        }
    }
    return true;
}

void MainWindow::newGame() {
    QList<QPushButton *> buttons = ui->gridLayoutWidget->findChildren<QPushButton *>();
    for(const auto button: buttons) {
        button->setEnabled(true);
        button->setText(" ");
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            field[i][j] = 'N';
        }
    }

    isPlayerTwoTurn = false;
}

