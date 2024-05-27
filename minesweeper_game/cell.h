#ifndef CELL_H
#define CELL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class Cell : public QWidget {
    Q_OBJECT



signals:
    void clicked();

public:

    enum Mode {
        Empty, Flag, Mine, Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Hint, WrongFlag
    };

    explicit Cell(int row, int col, int numRows, int numCols, Cell* (*cells)[10], QWidget *parent = nullptr);
    void setMode(Mode newMode);
    bool hasMine() const { return mode == Mine; }
    void setMine() { setMode(Mine); }
    void increaseCount();
    void reveal();
    bool isRevealed() const { return revealed; }
    void resetRevealed() { revealed = false; }
    bool isMine() const { return mode == Mine; }
    void lockCell(); // Method to lock the cell from further clicks
    Mode currentMode() const { return mode; }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void updateImage();
    void revealEmptyNeighbors(int row, int col);
    Mode mode;
    QLabel *imageLabel;
    bool revealed;
    int numRows;
    int numCols;
    Cell* (*cells)[10];
    int row;
    int col;
};

#endif // CELL_H
