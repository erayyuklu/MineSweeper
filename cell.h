#ifndef CELL_H
#define CELL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QWidget>

class Cell : public QWidget {
    Q_OBJECT
    bool willRevealIfNotRevealed = false;
    bool hint = false;

signals:
    void clicked();
    void rightClicked();

public:
    enum Mode {
        Empty,
        Flag,
        Mine,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Hint,
        WrongFlag
    };

    Mode currentMode() const { return mode; }

    static void setHintButton(QPushButton *button);

    explicit Cell(int row, int col, int numRows, int numCols, Cell *(**cells),
                  bool &gameOver,
                  void (*lockAllCells)(Cell ***cells, int numRows, int numCols),
                  void (*openAllMines)(Cell ***cells, int numRows, int numCols),
                  QWidget *parent = nullptr);
    void setMode(Mode newMode);

    bool isWillRevealIfNotRevealed() const { return willRevealIfNotRevealed; }
    bool isGuaranteedMine() const { return guaranteedMine; }
    bool isHint() const { return mode == Hint; }
    bool isSafe() const { return safe; }
    bool isMine() const { return mode == Mine; }
    bool isRevealed() const { return revealed; }
    bool isFlagged() const { return flagged; }
    bool hasMine() const { return mode == Mine; }

    void hideFlag();
    void showFlag();
    void revealIfHinted() { if (hint) reveal();}
    void markAsHint() { hint = true; }
    void resetHint() { hint = false; }
    void markAsWillRevealIfNotRevealed() { willRevealIfNotRevealed = true; }
    void resetWillRevealIfNotRevealed() { willRevealIfNotRevealed = false; }
    void resetSafe() { safe = false; }
    void resetGuaranteedMine() { guaranteedMine = false; }
    void setHint() { setMode(Hint); }
    void setSafe(bool value) { safe = value; }
    void setGuaranteedMine(bool value) { guaranteedMine = value; }
    void setMine() { setMode(Mine); }
    void increaseCount();
    void reveal();
    void setFlagged(bool value) { flagged = value; }
    void resetRevealed() { revealed = false; }
    void lockCell();  // Method to lock the cell from further clicks
    void resetCell();
    void showHint();



protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    static QPushButton *hintButton;  // Add this line
    Mode mode;
    QLabel *imageLabel;
    Cell *(**cells);

    bool safe;
    bool guaranteedMine;
    bool revealed;
    bool flagged;
    bool &gameOver;

    int numRows;
    int numCols;
    int row;
    int col;

    void checkWinCondition();
    void updateImage();
    void revealEmptyNeighbors(int row, int col);
    void handleRightClick();  // Method to handle right-click events
    void (*lockAllCells)(Cell ***cells, int numRows, int numCols);
    void (*openAllMines)(Cell ***cells, int numRows, int numCols);
};

#endif  // CELL_H
