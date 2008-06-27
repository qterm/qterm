#include "testglobal.h"
#include "qtermglobal.h"
#include <QtDebug>

using namespace QTerm;

void TestGlobal::testGlobal()
{
    qDebug() << Global::instance()->loadNameList();
}

QTEST_MAIN(TestGlobal)
#include "testglobal.moc"

