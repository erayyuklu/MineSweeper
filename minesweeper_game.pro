SOURCES += \
    cell.cpp \
    main.cpp \
    utils.cpp

QT += core widgets gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = minesweeper
TEMPLATE = app


HEADERS += \
    cell.h \
    utils.h

# Add the images folder to the resources
RESOURCES += \
    images.qrc \
    images.qrc
