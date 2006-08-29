#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "qtermdecode.h"
#include "qtermbuffer.h"
#include "qtermtextline.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QTermBuffer buffer(120,120,200);
	QTermDecode decoder(&buffer);

	QFile file("main.cpp");
	file.open(QIODevice::ReadOnly);

	QByteArray ba=file.readAll();

	decoder.decode(ba.data(),ba.size());

	QFile of("pp.txt");
	of.open(QIODevice::WriteOnly);

	QTextStream ds(&of);
	
	int n=0;
	while(n<buffer.lines())
	{ 
		QByteArray cstrTemp = buffer.at(n)->getText();
		if(!cstrTemp.isEmpty())
			ds << cstrTemp;
		else
			ds << "\n";
		n++;
	}
	of.close();
	app.quit();
}
