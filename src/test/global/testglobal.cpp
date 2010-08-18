#include "testglobal.h"
#include "qtermglobal.h"
#include <QtDebug>

using namespace QTerm;

void TestGlobal::testGlobal()
{
    QDomDocument doc = Global::instance()->addrXml();
    qDebug() << Global::instance()->loadFavoriteList(doc);
}

QTEST_MAIN(TestGlobal)
#include "testglobal.moc"

