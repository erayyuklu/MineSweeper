#include "utils.h"

/*
 * Decides whether cells are guaranteed to be safe or mines from the perspective
 * of a user. Used in the hint algorithm. If the number of guaranteed mines
 * around a cell equals the cell's number, the remaining hidden cells around it
 * are marked as safe. If the number of guaranteed mines plus hidden cells
 * around a cell equals the cell's number, the remaining hidden cells are marked
 * as mines.
 */
void updateSafeAndMineCells(Cell ***cells, int numRows, int numCols,
                            bool &changed) {
    changed = false;
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            Cell *cell = cells[i][j];
            if (cell->isRevealed() && cell->currentMode() >= Cell::Num1 &&
                cell->currentMode() <= Cell::Num8) {
                int num = cell->currentMode() - Cell::Num0;

                int mineCount = 0;
                int hiddenCount = 0;
                QVector<Cell *> neighbors;

                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (di == 0 && dj == 0)
                            continue;  // Skip the cell itself
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < numRows && nj >= 0 &&
                            nj < numCols) {
                            Cell *neighbor = cells[ni][nj];
                            if (!neighbor->isRevealed()) {
                                hiddenCount++;
                                neighbors.append(neighbor);
                                if (neighbor->isGuaranteedMine()) {
                                    mineCount++;
                                }
                            }
                        }
                    }
                }
                if (mineCount == num) {
                    for (Cell *neighbor : neighbors) {
                        if (!neighbor->isGuaranteedMine() &&
                            !neighbor->isSafe()) {
                            neighbor->setSafe(true);
                            changed = true;
                        }
                    }
                }

                if (mineCount + hiddenCount == num) {
                    for (Cell *neighbor : neighbors) {
                        if (!neighbor->isSafe() &&
                            !neighbor->isGuaranteedMine()) {
                            neighbor->setGuaranteedMine(true);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

/*
 * Provides a hint to the player by identifying a safe cell that hasn't been
 * revealed. It updates the status of cells using the updateSafeAndMineCells
 * function until no changes occur. If a safe cell is found, it is marked as a
 * hint. If no safe cells are available, a message is displayed.
 */
void giveHint(Cell ***cells, int numRows, int numCols) {
    bool changed;
    do {
        updateSafeAndMineCells(cells, numRows, numCols, changed);
    } while (changed);

    static Cell *hintCell = nullptr;

    Cell *newHintCell = nullptr;
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            Cell *cell = cells[i][j];
            if (cell->isSafe() && !cell->isRevealed() && !cell->isHint()) {
                cell->showHint();    // Show hint without changing the mode
                cell->markAsHint();  // Mark the cell as suggested by the hint
                newHintCell = cell;  // Store the new hint cell
                break;
            }
        }
        if (newHintCell) break;
    }

    if (!newHintCell) {
        QMessageBox::information(nullptr, "Hint", "No safe moves found!");
    }

    if (hintCell && !hintCell->isRevealed()) {
        hintCell->revealIfHinted();
    }
    hintCell = newHintCell;
}

/*
 * Locks all cells on the game board, preventing any further interactions.
 * This is useful for ending the game or preventing changes during certain
 * operations.
 */
void lockAllCells(Cell ***cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            cells[i][j]->lockCell();  // Lock the cell using its lockCell method
        }
    }
}

/*
 * Randomly places mines on the game board, ensuring that each cell can only
 * contain one mine. The number of mines placed is determined by the numMines
 * parameter.
 */
void placeMines(Cell ***cells, int numRows, int numCols, int numMines) {
    srand(time(nullptr));
    int placedMines = 0;
    while (placedMines < numMines) {
        int row = rand() % numRows;
        int col = rand() % numCols;
        if (!cells[row][col]->hasMine()) {
            cells[row][col]->setMine();
            placedMines++;
        }
    }
}

/*
 * Sets the number for each cell based on the number of surrounding mines.
 * Cells without mines are assigned a number indicating how many mines are
 * adjacent to them.
 */
void setNumbers(Cell ***cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            if (!cells[i][j]->hasMine()) {
                int mineCount = 0;
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < numRows && nj >= 0 &&
                            nj < numCols && cells[ni][nj]->hasMine()) {
                            mineCount++;
                        }
                    }
                }
                cells[i][j]->setMode(
                    static_cast<Cell::Mode>(Cell::Num0 + mineCount));
            }
        }
    }
}

/*
 * Reveals all mines on the game board, typically used when the game is lost.
 * Temporarily disconnects the clicked signal to prevent interaction during the
 * reveal process.
 */
void openAllMines(Cell ***cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            if (cells[i][j]->hasMine()) {
                cells[i][j]->disconnect();  // Temporarily disconnect the
                    // clicked signal
                cells[i][j]->reveal();
            }
        }
    }
}

/*
 * Resets the hint state for all cells on the game board.
 * This is useful for clearing any hint markers before a new hint is provided.
 */
void resetHintState(Cell ***cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            cells[i][j]->resetHint();  // Reset hint state for all cells
        }
    }
}

/*
 * Cleans up the dynamically allocated memory for the game board.
 * Deletes each cell and frees the memory allocated for the rows and the cell
 * array.
 */
void cleanup(Cell ***cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            delete cells[i][j];
        }
        delete[] cells[i];
    }
    delete[] cells;
}

/*
 * Updates the score label with the current score.
 * The score is incremented by the number of revealed cells and displayed on the
 * score label.
 */
void updateScoreLabel(QLabel *scoreLabel, int revealedCount, int *score) {
    *score += revealedCount;
    scoreLabel->setText(QString("Score: %1").arg(*score));
}
