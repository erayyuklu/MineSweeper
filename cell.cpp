#include <QMessageBox>
#include <QPixmap>

#include "cell.h"

Cell::Cell(int row, int col, int numRows, int numCols, Cell *(**cells),
           bool &gameOver,
           void (*lockAllCells)(Cell ***cells, int numRows, int numCols),
           void (*openAllMines)(Cell ***cells, int numRows, int numCols),
           QWidget *parent)
    : QWidget(parent),
    mode(Empty),
    cells(cells),
    guaranteedMine(false),
    revealed(false),
    gameOver(gameOver),
    numRows(numRows),
    numCols(numCols),
    row(row),
    col(col),
    lockAllCells(lockAllCells),
    openAllMines(openAllMines) {
    imageLabel = new QLabel(this);
}

QPushButton *Cell::hintButton = nullptr;  // Initialize the static member

void Cell::setHintButton(QPushButton *button) { hintButton = button; }

void Cell::showHint() {
    QPixmap pixmap(":/images/hint.png");
    imageLabel->setPixmap(pixmap);
}

void Cell::showFlag() {
    QPixmap pixmap(":/images/flag.png");
    imageLabel->setPixmap(pixmap);
}
void Cell::hideFlag() {
    QPixmap pixmap(":/images/empty.png");
    imageLabel->setPixmap(pixmap);
}

void Cell::mousePressEvent(QMouseEvent *event) {
    if (!isEnabled())  // If cell is already locked, ignore the click
        return;

    if (event->button() == Qt::LeftButton) {
        if (mode == Mine) {
            gameOver = true;  // Set game state to over
            if (hintButton) {
                hintButton->setEnabled(false);  // Disable the hint button
            }
            openAllMines(cells, numRows, numCols);  // Open all mines
            QMessageBox::information(this, "Game Over",
                                     "You Lost!");  // Show "You Lost" message
            lockAllCells(cells, numRows, numCols);  // Lock all cells
        } else {
            reveal();  // Proceed with normal revealing logic
        }
    }

    if (event->button() == Qt::RightButton) {
        if (!isEnabled() || revealed ||
            gameOver)  // If cell is already locked, revealed, or game is over,
            // ignore the click
            return;
        if (!isFlagged()) {
            showFlag();
            setFlagged(true);
        } else {
            hideFlag();
            setFlagged(false);
        }
    }
}

/*
 * Sets the mode of the cell and updates its image accordingly.
 * If the cell is already revealed, it immediately updates the image.
 */
void Cell::setMode(Mode newMode) {
    mode = newMode;
    if (revealed) {
        updateImage();
    } else {
        QPixmap pixmap(":/images/empty.png");
        imageLabel->setPixmap(pixmap);
    }
    updateImage();
}

/*
 * Updates the cell's image based on its mode and revealed status.
 */
void Cell::updateImage() {
    QPixmap pixmap;
    if (revealed) {
        switch (mode) {
        case WrongFlag:
            pixmap.load(":/images/wrong-flag.png");
            break;
        case Mine:
            pixmap.load(":/images/mine.png");
            break;
        case Hint:
            pixmap.load(":/images/hint.png");
            break;
        case Flag:
            pixmap.load(":/images/flag.png");
            break;
        case Empty:
            pixmap.load(":/images/empty.png");
            break;
        case Num8:
            pixmap.load(":/images/8.png");
            break;
        case Num7:
            pixmap.load(":/images/7.png");
            break;
        case Num6:
            pixmap.load(":/images/6.png");
            break;
        case Num5:
            pixmap.load(":/images/5.png");
            break;
        case Num4:
            pixmap.load(":/images/4.png");
            break;
        case Num3:
            pixmap.load(":/images/3.png");
            break;
        case Num2:
            pixmap.load(":/images/2.png");
            break;
        case Num1:
            pixmap.load(":/images/1.png");
            break;
        case Num0:
            pixmap.load(":/images/0.png");
            break;
        }
    } else {
        pixmap.load(":/images/empty.png");
    }
    imageLabel->setPixmap(pixmap);
}

/*
 * Increases the count of the cell if it is a number between Num0 and Num8.
 * If the cell is revealed, it updates the image to reflect the new count.
 */
void Cell::increaseCount() {
    if (mode >= Num0 && mode <= Num8) {
        mode = static_cast<Mode>(mode + 1);
        if (revealed) {
            updateImage();
        }
    }
}

/*
 * Checks if the player has won the game.
 * If all non-mine cells are revealed, the player wins and a win message is
 * shown.
 */
void Cell::checkWinCondition() {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            if (!cells[i][j]->isRevealed() && !cells[i][j]->hasMine()) {
                return;  // If there are still non-mine cells that are not
                    // revealed, return
            }
        }
    }

    if (!gameOver) {
        gameOver = true;
        if (hintButton) {
            hintButton->setEnabled(false);
        }
        openAllMines(cells, numRows, numCols);
        QMessageBox::information(this, "Game Won", "Congratulations, you won!");
        lockAllCells(cells, numRows, numCols);
    }
}

/*
 * Reveals the cell, updating its image and emitting the clicked signal.
 * If the cell is empty or contains a zero, it reveals its neighboring cells.
 * Also checks for win condition after revealing the cell.
 */
void Cell::reveal() {
    if (revealed) {
        return;  // Do not reveal if already revealed
    }
    revealed = true;
    updateImage();
    emit clicked();  // Emit the clicked signal

    if (mode == Empty || mode == Num0) {
        revealEmptyNeighbors(row, col);
    }

    checkWinCondition();  // Check if the player has won after revealing a cell
}

/*
 * Locks the cell, disabling mouse events and changing the cursor.
 */
void Cell::lockCell() {
    setCursor(Qt::ArrowCursor);
    setEnabled(false);  // Disable mouse events
}

/*
 * Resets the cell to its initial state.
 * Disconnects all signals, sets the mode to Empty, resets flags, and updates
 * the image.
 */
void Cell::resetCell() {
    disconnect();            // Disconnect all signals
    setMode(Empty);          // Set mode to Empty
    revealed = false;        // Set revealed to false
    setEnabled(true);        // Enable the cell for interaction
    safe = false;            // Reset the safe attribute
    guaranteedMine = false;  // Reset the guaranteedMine attribute
    updateImage();           // Update the cell image
    flagged = false;
}

/*
 * Reveals all neighboring cells that are not yet revealed.
 * This function is called recursively for empty cells to reveal large empty
 * areas.
 */
void Cell::revealEmptyNeighbors(int row, int col) {
    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            int ni = row + di;
            int nj = col + dj;
            if (ni >= 0 && ni < numRows && nj >= 0 && nj < numCols &&
                !cells[ni][nj]->isRevealed()) {
                cells[ni][nj]->reveal();
            }
        }
    }
}
