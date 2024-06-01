#ifndef CELL_H
#define CELL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>

// Forward declaration of RightClickHandler class
class RightClickHandler;

class Cell : public QWidget {
    Q_OBJECT

signals:
    void clicked();
    void rightClicked();


public:
    void resetSafe() { safe = false; }
    void resetGuaranteedMine() { guaranteedMine = false; }
    static void setHintButton(QPushButton *button); // Add this line


    bool isHint() const { return mode == Hint; }
    void setHint() { setMode(Hint); }
    bool isSafe() const { return safe; }
    void setSafe(bool value) { safe = value; }
    bool isGuaranteedMine() const { return guaranteedMine; }
    void setGuaranteedMine(bool value) { guaranteedMine = value; }
    enum Mode {
        Empty, Flag, Mine, Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Hint, WrongFlag
    };

    explicit Cell(int row, int col, int numRows, int numCols, Cell* (**cells), bool &gameOver, void (*lockAllCells)(Cell*** cells, int numRows, int numCols), void (*openAllMines)(Cell*** cells, int numRows, int numCols), QWidget *parent = nullptr);
    void setMode(Mode newMode);
    bool hasMine() const { return mode == Mine; }
    void setMine() { setMode(Mine); }
    void increaseCount();
    void reveal();
    bool isRevealed() const { return revealed; }
    void resetRevealed() { revealed = false; }
    bool isMine() const { return mode == Mine; }
    void lockCell(); // Method to lock the cell from further clicks
    void resetCell();
    void showHint();

    Mode currentMode() const { return mode; }
    RightClickHandler* getRightClickHandler() { return rightClickHandler; }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    static QPushButton *hintButton; // Add this line
    bool safe;
    bool guaranteedMine;
    void checkWinCondition();
    void updateImage();
    void revealEmptyNeighbors(int row, int col);
    void handleRightClick(); // Method to handle right-click events
    Mode mode;
    QLabel *imageLabel;
    bool revealed;
    int numRows;
    int numCols;
    Cell* (**cells);
    int row;
    int col;
    bool &gameOver;
    void (*lockAllCells)(Cell*** cells, int numRows, int numCols);
    void (*openAllMines)(Cell*** cells, int numRows, int numCols);
    RightClickHandler* rightClickHandler;
};

#endif // CELL_H
