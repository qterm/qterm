#include "chartable.h"

#include <QScrollArea>
#include <QApplication>

int main (int argc, char **argv)
{
    QApplication app(argc, argv);

    QScrollArea scroll;
    scroll.setWidgetResizable(true);
    QTerm::CharTable table;
    scroll.setWidget(&table);

    scroll.show();

    app.exec();
}
