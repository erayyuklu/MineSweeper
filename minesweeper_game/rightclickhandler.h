#ifndef RIGHTCLICKHANDLER_H
#define RIGHTCLICKHANDLER_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include "cell.h"

class RightClickHandler : public QObject {
    Q_OBJECT
public:
    explicit RightClickHandler(Cell* cell);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void rightClicked();

private:
    Cell* m_cell;
};

#endif // RIGHTCLICKHANDLER_H
