#ifndef UTILS_H
#define UTILS_H

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "cell.h"

void updateSafeAndMineCells(Cell ***cells, int numRows, int numCols,
                            bool &changed);
void giveHint(Cell ***cells, int numRows, int numCols);
void lockAllCells(Cell ***cells, int numRows, int numCols);
void placeMines(Cell ***cells, int numRows, int numCols, int numMines);
void setNumbers(Cell ***cells, int numRows, int numCols);
void openAllMines(Cell ***cells, int numRows, int numCols);
void resetHintState(Cell ***cells, int numRows, int numCols);
void cleanup(Cell ***cells, int numRows, int numCols);

void updateScoreLabel(QLabel *scoreLabel, int revealedCount, int *score);

#endif  // UTILS_H
