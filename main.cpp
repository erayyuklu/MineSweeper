#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "cell.h"
#include "utils.h"

// Config Variables
const int N = 20;
const int M = 20;
const int K = 20;

// UI Variables
const int buttonWidth = 56;
const int cellSize = 15;
const int paddingX = 20;
const int paddingY = 64;

// State Variables
bool gameOver = false;
int score = 0;  // Initialize score variable

int main(int argc, char *argv[]) {
    // Global application-level configuration
    QApplication app(argc, argv);
    QWidget mainWindow;
    QIcon appIcon(":/images/mine.png");
    mainWindow.setWindowTitle("Minesweeper");
    app.setWindowIcon(appIcon);

    // Window configuration
    mainWindow.setFixedSize(cellSize * N + paddingX, cellSize * M + paddingY);
    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWindow);
    QHBoxLayout *topLayout = new QHBoxLayout;
    mainLayout->addLayout(topLayout);

    // Score label
    QLabel *scoreLabel = new QLabel("Score: 0", &mainWindow);
    topLayout->addWidget(scoreLabel);

    // Restart button
    QPushButton *restartButton = new QPushButton("Restart", &mainWindow);
    restartButton->setFixedWidth(
        buttonWidth);  // Adjust the width of the restart button
    topLayout->addWidget(restartButton);

    // Hint button
    QPushButton *hintButton = new QPushButton("Hint", &mainWindow);
    hintButton->setFixedWidth(buttonWidth);
    topLayout->addWidget(hintButton);
    Cell::setHintButton(hintButton);

    // Grid of game cells
    QGridLayout *gridLayout = new QGridLayout;
    mainLayout->addLayout(gridLayout);
    gridLayout->setSpacing(0);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setRowStretch(0, 0);
    gridLayout->setColumnMinimumWidth(15, 15);
    gridLayout->setRowMinimumHeight(15, 15);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < N; ++i) {
        gridLayout->setRowStretch(i, 0);
        gridLayout->setRowMinimumHeight(
            i, 15);
    }
    for (int j = 0; j < M; ++j) {
        gridLayout->setColumnStretch(j, 0);
        gridLayout->setColumnMinimumWidth(
            j, 15);
    }

    Cell ***cells = new Cell **[N];
    for (int i = 0; i < N; ++i) {
        cells[i] = new Cell *[M];
    }

    // Create and add cells to the grid layout
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            cells[i][j] = new Cell(i, j, N, M, cells, gameOver, lockAllCells,
                                   openAllMines);
            cells[i][j]->setMode(Cell::Empty);
            gridLayout->addWidget(cells[i][j], i, j);
            QObject::connect(cells[i][j], &Cell::clicked, [&scoreLabel]() {
                updateScoreLabel(scoreLabel, 1, &score);
            });
        }
    }

    placeMines(cells, N, M, K);
    setNumbers(cells, N, M);

    // Connect the restart button's clicked signal to a slot to restart the game
    QObject::connect(
        restartButton, &QPushButton::clicked,
        [&cells, &scoreLabel, &hintButton]() {
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < M; ++j) {
                    cells[i][j]->resetCell();  // Use the resetCell method

                    // Connect the reveal signal to update the score
                    QObject::connect(
                        cells[i][j], &Cell::clicked, [&scoreLabel]() {
                            updateScoreLabel(scoreLabel, 1, &score);
                        });
                }
            }
            score = 0;
            gameOver = false;
            scoreLabel->setText("Score: 0");
            hintButton->setEnabled(true);
            placeMines(cells, N, M, K);
            setNumbers(cells, N, M);
            resetHintState(cells, N, M);
        });

    QObject::connect(hintButton, &QPushButton::clicked,
                     [cells]() { giveHint(cells, N, M); });

    mainWindow.setLayout(mainLayout);
    mainWindow.show();
    int result = app.exec();
    cleanup(cells, N, M);
    return result;
}
