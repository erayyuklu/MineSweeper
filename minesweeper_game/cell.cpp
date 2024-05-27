#include "cell.h"
#include <QPixmap>
#include <QMessageBox>

Cell::Cell(int row, int col, int numRows, int numCols, Cell* (*cells)[10], QWidget *parent)
    : QWidget(parent), mode(Empty), revealed(false), numRows(numRows), numCols(numCols), cells(cells), row(row), col(col) {
    imageLabel = new QLabel(this); // Set this as the parent
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

void Cell::reveal() {
    revealed = true;
    updateImage();
    if (mode == Empty) {
        revealEmptyNeighbors(row, col);
    } else if (mode == Num0) {
        revealEmptyNeighbors(row, col);
    }
}




void Cell::mousePressEvent(QMouseEvent *event) {
    if (!isEnabled()) // If cell is already locked, ignore the click
        return;

    if (event->button() == Qt::LeftButton) {
        emit clicked(); // Emit the clicked signal
        if (mode == Mine) {

            lockCell(); // Lock all cells
        } else {
            reveal(); // Proceed with normal revealing logic
        }
    }
}

void Cell::lockCell() {
    setEnabled(false); // Disable mouse events
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
