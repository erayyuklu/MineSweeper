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
    mainWindow.resize(200, 220); // Set width and height according to your preference

    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWindow);

    // Create a horizontal layout for the score and restart button
    QHBoxLayout *topLayout = new QHBoxLayout;

    // Create a score label
    QLabel *scoreLabel = new QLabel("Score: 0", &mainWindow);
    topLayout->addWidget(scoreLabel);



    // Create a restart button
    QPushButton *restartButton = new QPushButton("Restart", &mainWindow);
    restartButton->setFixedWidth(70); // Adjust the width of the restart button
    topLayout->addWidget(restartButton);

    // Create a Hint button
    QPushButton *hintButton = new QPushButton("Hint", &mainWindow);
    hintButton->setFixedWidth(70);
    topLayout->addWidget(hintButton);



    // Add the topLayout to the mainLayout
    mainLayout->addLayout(topLayout);

    // Create a grid layout to hold the cells
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0); // Set spacing between cells to 0
    gridLayout->setContentsMargins(0, 0, 0, 0); // Set margins to 0 to remove extra space



    // Define the size of the game table
    const int numRows = 11;
    const int numCols = 11;
    const int numMines = 5; // Define the number of mines to place



    // Create an array to hold the cells
    Cell ***cells = new Cell**[numRows];
    for (int i = 0; i < numRows; ++i) {
        cells[i] = new Cell*[numCols];
    }
    // Create and add cells to the grid layout
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {

            cells[i][j] = new Cell(i, j, numRows, numCols, cells, gameOver, lockAllCells, openAllMines);
            cells[i][j]->setMode(Cell::Empty); // Set initial mode to Empty
            cells[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Make cells expandable
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
    QObject::connect(restartButton, &QPushButton::clicked, [&cells, &scoreLabel, numRows, numCols, numMines](){
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
