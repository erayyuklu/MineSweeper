#include "rightclickhandler.h"

RightClickHandler::RightClickHandler(Cell* cell) : m_cell(cell) {}

bool RightClickHandler::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            emit rightClicked(); // Emit the custom right-click signal
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
