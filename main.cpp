#include <QApplication>

#include "qtermeditor.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QTermEditor *ed=new QTermEditor();

    ed->show();
    
    return app.exec();
}
