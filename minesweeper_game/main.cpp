#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "cell.h"
#include <ctime>
#include <cstdlib>
#include <QMessageBox>
#include "rightclickhandler.h"

bool gameOver = false;

void lockAllCells(Cell* cells[][10], int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            cells[i][j]->setEnabled(false); // Disable mouse events for the cell
        }
    }
}

void placeMines(Cell *cells[][10], int numRows, int numCols, int numMines) {
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

void setNumbers(Cell *cells[][10], int numRows, int numCols) {
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

void openAllMines(Cell* cells[][10], int numRows, int numCols) {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            if (cells[i][j]->hasMine()) {
                cells[i][j]->reveal();
            }
        }
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create a main window
    QWidget mainWindow;
    mainWindow.setWindowTitle("Minesweeper");

    // Set initial size of the main window
    mainWindow.resize(200, 220); // Set width and height according to your preference

    // Create a vertical layout for the main window
    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWindow);

    // Create a restart button
    QPushButton *restartButton = new QPushButton("Restart", &mainWindow);
    mainLayout->addWidget(restartButton);

    // Create a grid layout to hold the cells
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0); // Set spacing between cells to 0
    gridLayout->setContentsMargins(5, 5, 5, 5); // Set margins to 0 to remove extra space

    // Define the size of the game table
    const int numRows = 10;
    const int numCols = 10;
    const int numMines = 10; // Define the number of mines to place



    // Create an array to hold the cells
    Cell *cells[numRows][numCols];

    // Create and add cells to the grid layout
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            cells[i][j] = new Cell(i, j, numRows, numCols, cells, gameOver, lockAllCells, openAllMines);
            cells[i][j]->setMode(Cell::Empty); // Set initial mode to Empty
            cells[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Make cells expandable
            gridLayout->addWidget(cells[i][j], i, j);
        }
    }


    // Place mines and set numbers
    placeMines(cells, numRows, numCols, numMines);
    setNumbers(cells, numRows, numCols);

    // Add the grid layout to the main layout
    mainLayout->addLayout(gridLayout);

    // Connect the restart button's clicked signal to a slot to restart the game
    QObject::connect(restartButton, &QPushButton::clicked, [&cells, numRows, numCols, numMines](){
        // Reset all cells to Empty mode and update the UI
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                cells[i][j]->setMode(Cell::Empty);
                cells[i][j]->setEnabled(true); // Re-enable mouse events for the cell
                cells[i][j]->resetRevealed(); // Reset the revealed flag
            }
        }

        // Redistribute mines and set numbers
        placeMines(cells, numRows, numCols, numMines);
        setNumbers(cells, numRows, numCols);

        // Reset game state flag
        gameOver = false;
    });

    // Connect the cells to the lockCell() method if the game is not over
    // Connect the rightClicked signal of each cell to the appropriate slot

    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            // Get the RightClickHandler object associated with the cell
            RightClickHandler* rightClickHandler = cells[i][j]->getRightClickHandler();

            // Capture gameOver by reference in the lambda capture list
            bool& gameOverRef = gameOver; // Capture gameOver by reference
            QObject::connect(rightClickHandler, &RightClickHandler::rightClicked, cells[i][j], [cells, i, j, &gameOverRef]() {
                if (!gameOverRef) {
                    // Toggle between Flag and Empty modes
                    if (cells[i][j]->currentMode() == Cell::Empty) {
                        cells[i][j]->setMode(Cell::Flag); // Set mode to Flag
                    } else if (cells[i][j]->currentMode() == Cell::Flag) {
                        cells[i][j]->setMode(Cell::Empty); // Set mode to Empty
                    }
                }
            });
        }
    }









    // Set the main window's layout
    mainWindow.setLayout(mainLayout);

    // Show the main window
    mainWindow.show();

    // Execute the application
    return app.exec();
}
