#include "cell.h"
#include "rightclickhandler.h"
#include <QPixmap>
#include <QMessageBox>

Cell::Cell(int row, int col, int numRows, int numCols, Cell* (**cells), bool &gameOver, void (*lockAllCells)(Cell*** cells, int numRows, int numCols), void (*openAllMines)(Cell*** cells, int numRows, int numCols), QWidget *parent)
    : QWidget(parent), mode(Empty), revealed(false), numRows(numRows), numCols(numCols), cells(cells), row(row), col(col), gameOver(gameOver), lockAllCells(lockAllCells), openAllMines(openAllMines) {
    imageLabel = new QLabel(this); // Set this as the parent

    rightClickHandler = new RightClickHandler(this);
    this->installEventFilter(rightClickHandler);

    connect(rightClickHandler, &RightClickHandler::rightClicked, this, &Cell::handleRightClick);
}

void Cell::mousePressEvent(QMouseEvent *event) {
    if (!isEnabled()) // If cell is already locked, ignore the click
        return;

    if (event->button() == Qt::LeftButton) {
        if (mode == Mine) {
            gameOver = true; // Set game state to over

            openAllMines(cells, numRows, numCols); // Open all mines
            QMessageBox::information(this, "Game Over", "You Lost!"); // Show "You Lost" message
            lockAllCells(cells, numRows, numCols); // Lock all cells

        } else {
            reveal(); // Proceed with normal revealing logic
        }
    }
}

void Cell::handleRightClick() {
    if (!isEnabled() || revealed || gameOver) // If cell is already locked, revealed, or game is over, ignore the click
        return;

    // Toggle between Flag and Empty modes
    if (mode == Empty) {
        setMode(Flag); // Set mode to Flag
    } else if (mode == Flag) {
        setMode(Empty); // Set mode to Empty
    }
}

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

    // Set the pixmap to the label
    imageLabel->setPixmap(pixmap);
}

void Cell::increaseCount() {
    if (mode >= Num0 && mode <= Num8) {
        mode = static_cast<Mode>(mode + 1);
        if (revealed) {
            updateImage();
        }
    }
}

void Cell::checkWinCondition() {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            if (!cells[i][j]->isRevealed() && !cells[i][j]->hasMine()) {
                return; // If there are still non-mine cells that are not revealed, return
            }
        }
    }

    // If all non-mine cells are revealed, the player has won
    if (!gameOver) { // Ensure this block only runs once
        gameOver = true; // Set game state to over
        openAllMines(cells, numRows, numCols); // Open all mines
        QMessageBox::information(this, "Game Won", "Congratulations, you won!"); // Show win message
        lockAllCells(cells, numRows, numCols); // Lock all cells
    }
}


void Cell::reveal() {
    if (revealed) {
        return; // Do not reveal if already revealed
    }

    revealed = true;
    updateImage();
    emit clicked(); // Emit the clicked signal

    if (mode == Empty || mode == Num0) {
        revealEmptyNeighbors(row, col);
    }

    checkWinCondition(); // Check if the player has won after revealing a cell
}
void Cell::lockCell() {
    setCursor(Qt::ArrowCursor);
    setEnabled(false); // Disable mouse events

}

void Cell::resetCell() {
    disconnect(); // Disconnect all signals
    setMode(Empty); // Set mode to Empty
    revealed = false; // Set revealed to false
    setEnabled(true); // Enable the cell for interaction
    updateImage(); // Update the cell image
}

void Cell::revealEmptyNeighbors(int row, int col) {
    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            int ni = row + di;
            int nj = col + dj;
            if (ni >= 0 && ni < numRows && nj >= 0 && nj < numCols && !cells[ni][nj]->isRevealed()) {
                cells[ni][nj]->reveal();
            }
        }
    }
}
