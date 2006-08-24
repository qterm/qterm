#include "qterm.h"
#include "qtermframe.h"

#include <QApplication>

int main( int argc, char ** argv ) 
{
    Q_INIT_RESOURCE(frame);
	QApplication app( argc, argv );
	iniWorkingDir();
	QTermFrame frame;
	frame.show();
	return app.exec();
}
