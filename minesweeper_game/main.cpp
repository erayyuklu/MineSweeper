#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "cell.h"
#include <ctime>
#include <cstdlib>
#include <QMessageBox>


bool gameOver = false;
int N=20;
int M=20;
int K=15;


#include <QDebug>

void updateSafeAndMineCells(Cell*** cells, int numRows, int numCols, bool& changed) {
    changed = false;

    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            Cell* cell = cells[i][j];
            if (cell->isRevealed() && cell->currentMode() >= Cell::Num1 && cell->currentMode() <= Cell::Num8) {
                int num = cell->currentMode() - Cell::Num0;

                int mineCount = 0;
                int hiddenCount = 0;
                QVector<Cell*> neighbors;

                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (di == 0 && dj == 0) continue; // Skip the cell itself
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < numRows && nj >= 0 && nj < numCols) {
                            Cell* neighbor = cells[ni][nj];
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

                qDebug() << "Cell(" << i << "," << j << "): number =" << num << ", mineCount =" << mineCount << ", hiddenCount =" << hiddenCount;

                if (mineCount == num) {
                    for (Cell* neighbor : neighbors) {
                        if (!neighbor->isGuaranteedMine() && !neighbor->isSafe()) {
                            neighbor->setSafe(true);
                            qDebug() << "Cell(" << (neighbor - cells[0][0]) / numCols << "," << (neighbor - cells[0][0]) % numCols << ") set as Safe";
                            changed = true;
                        }
                    }
                }

                if (mineCount + hiddenCount == num) {
                    for (Cell* neighbor : neighbors) {
                        if (!neighbor->isSafe() && !neighbor->isGuaranteedMine()) {
                            neighbor->setGuaranteedMine(true);
                            qDebug() << "Cell(" << (neighbor - cells[0][0]) / numCols << "," << (neighbor - cells[0][0]) % numCols << ") set as Guaranteed Mine";
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

void giveHint(Cell*** cells, int numRows, int numCols) {
    bool changed;
    do {
        updateSafeAndMineCells(cells, numRows, numCols, changed);
    } while (changed);

    static Cell* hintCell = nullptr; // Store the previously suggested hint cell

    // Find a safe cell that hasn't been revealed
    Cell* newHintCell = nullptr;
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            Cell* cell = cells[i][j];
            if (cell->isSafe() && !cell->isRevealed() && !cell->isHint()) {
                cell->showHint(); // Show hint without changing the mode
                cell->markAsHint(); // Mark the cell as suggested by the hint
                newHintCell = cell; // Store the new hint cell
                break;
            }
        }
        if (newHintCell) break;
    }

    // If no safe cell is found, show a message
    if (!newHintCell) {
        QMessageBox::information(nullptr, "Hint", "No safe moves left!");
    }

    // If a previous hint cell exists and it hasn't been revealed, reveal it
    if (hintCell && !hintCell->isRevealed()) {
        hintCell->revealIfHinted();
    }

    // Update the previous hint cell with the new hint cell
    hintCell = newHintCell;
}





void lockAllCells(Cell*** cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            cells[i][j]->lockCell(); // Lock the cell using its lockCell method
        }
    }
}


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

void setNumbers(Cell ***cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            if (!cells[i][j]->hasMine()) {
                int mineCount = 0;
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < numRows && nj >= 0 && nj < numCols && cells[ni][nj]->hasMine()) {
                            mineCount++;
                        }
                    }
                }
                cells[i][j]->setMode(static_cast<Cell::Mode>(Cell::Num0 + mineCount));
            }
        }
    }
}



void openAllMines(Cell*** cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            if (cells[i][j]->hasMine()) {
                cells[i][j]->disconnect(); // Temporarily disconnect the clicked signal
                cells[i][j]->reveal();
            }
        }
    }
}


int score = 0; // Initialize score variable

void updateScoreLabel(QLabel* scoreLabel, int revealedCount) {
    score += revealedCount;
    scoreLabel->setText(QString("Score: %1").arg(score));
}
// In the main.cpp file
Cell* hintCell = nullptr;
// Function to reset the game
void resetHintState(Cell*** cells, int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            cells[i][j]->resetHint(); // Reset hint state for all cells
        }
    }
}


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create a main window
    QWidget mainWindow;
    mainWindow.setWindowTitle("Minesweeper");

    // Load the icon from a file
    QIcon appIcon(":/images/mine.png");

    // Set the application icon
    app.setWindowIcon(appIcon);

    // Set initial size of the main window
    mainWindow.setFixedSize(15*N + 22, 15*M + 52); // Set width and height according to your preference

    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWindow);

    // Create a horizontal layout for the score and restart button
    QHBoxLayout *topLayout = new QHBoxLayout;

    // Create a score label
    QLabel *scoreLabel = new QLabel("Score: 0", &mainWindow);
    topLayout->addWidget(scoreLabel);



    // Create a restart button
    QPushButton *restartButton = new QPushButton("Restart", &mainWindow);
    restartButton->setFixedWidth(48); // Adjust the width of the restart button
    topLayout->addWidget(restartButton);

    // Create a Hint button
    QPushButton *hintButton = new QPushButton("Hint", &mainWindow);
    hintButton->setFixedWidth(48);
    topLayout->addWidget(hintButton);



    // Add the topLayout to the mainLayout
    mainLayout->addLayout(topLayout);

    // Create a grid layout to hold the cells
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout -> setSpacing(0); // Set spacing between cells to 0
    gridLayout -> setColumnStretch(0,0);
    gridLayout -> setRowStretch(0,0);
    gridLayout -> setColumnMinimumWidth(15,15);
    gridLayout -> setRowMinimumHeight(15,15);
    gridLayout->setContentsMargins(0, 0, 0, 0); // Set margins to 0 to remove extra space

    for (int i = 0; i < N; ++i) {
        gridLayout->setRowStretch(i, 0);
        gridLayout->setRowMinimumHeight(i, 15); // Set the minimum height for each row
    }

    for (int j = 0; j < M; ++j) {
        gridLayout->setColumnStretch(j, 0);
        gridLayout->setColumnMinimumWidth(j, 15); // Set the minimum width for each column
    }



    // Define the size of the game table
    const int numRows = N;
    const int numCols = M;
    const int numMines = K; // Define the number of mines to place
    Cell::setHintButton(hintButton);


    // Create an array to hold the cells
    Cell ***cells = new Cell**[numRows];
    for (int i = 0; i < numRows; ++i) {
        cells[i] = new Cell*[numCols];
    }
    // Create and add cells to the grid layout
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {

            cells[i][j] = new Cell(i, j, numRows, numCols, cells, gameOver, lockAllCells, openAllMines);
            cells[i][j] -> setMode(Cell::Empty);
            gridLayout->addWidget(cells[i][j], i, j);

            // Connect the reveal signal to update the score
            QObject::connect(cells[i][j], &Cell::clicked, [&scoreLabel]() {
                updateScoreLabel(scoreLabel, 1);
            });
        }
    }

    // Remove spacing between cells
    gridLayout->setSpacing(0);

    // Add the grid layout to the main layout
    mainLayout->addLayout(gridLayout);

    // Place mines and set numbers
    placeMines(cells, numRows, numCols, numMines);
    setNumbers(cells, numRows, numCols);



    // Connect the restart button's clicked signal to a slot to restart the game
    QObject::connect(restartButton, &QPushButton::clicked, [&cells, &scoreLabel, &hintButton, numRows, numCols, numMines](){
        // Reset all cells to initial state
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                cells[i][j]->resetCell(); // Use the resetCell method

                // Connect the reveal signal to update the score
                QObject::connect(cells[i][j], &Cell::clicked, [&scoreLabel]() {
                    updateScoreLabel(scoreLabel, 1);
                });
            }
        }

        // Reset score and update the score label
        score = 0;
        scoreLabel->setText("Score: 0");

        // Re-enable the hint button
        hintButton->setEnabled(true);

        // Redistribute mines and set numbers
        placeMines(cells, numRows, numCols, numMines);
        setNumbers(cells, numRows, numCols);

        // Reset game state flag
        gameOver = false;

        // Reset hint state
        resetHintState(cells, numRows, numCols);
    });



    QObject::connect(hintButton, &QPushButton::clicked, [cells, numRows, numCols]() {
        giveHint(cells, numRows, numCols);
    });

    // Set the main window's layout
    mainWindow.setLayout(mainLayout);

    // Show the main window
    mainWindow.show();

    // Execute the application
    return app.exec();
}
