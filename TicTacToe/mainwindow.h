#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    bool isPlayerTwoTurn = false;
    char field[3][3];

    ~MainWindow();

public slots:
    void newGame();

private:
    char play(bool isPlayerTwoTurn);
    bool hasWinner(char field[3][3], char state);
    bool hasRowColumnWinner(char field[3][3], char state);
    bool hasPrimaryDiagonalWinner(char field[3][3], char state);
    bool hasSecondaryDiagonalWinner(char field[3][3], char state);
    bool isDraw(char field[3][3]);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
