#include "testconfig.h"
#include "qtermconfig.h"
#include <QtGui>
#include <QtDebug>
using namespace QTerm;

void TestConfig::initTestCase()
{
    conf= new Config("config.test");
}

void TestConfig::test()
{
    QVERIFY(conf->setItemValue("section","key","test"));
    QVERIFY(conf->save("config.test"));
    QString str = conf->getItemValue("section","key").toString();
    QVERIFY(str == "test");
    QVERIFY(conf->deleteItem("section","key"));
    QVERIFY(conf->save("config.test"));
    str = conf->getItemValue("section","key").toString();
    QVERIFY(str == "");
    QVERIFY(conf->setItemValue("section","key1","test"));
    QVERIFY(conf->setItemValue("section","key2","test"));
    QVERIFY(conf->save("config.test"));
    //QVERIFY(conf->deleteSection("section"));
    QVERIFY(conf->hasSection("section"));
    QVERIFY(conf->renameSection("section", "another section"));
    QVERIFY(conf->hasSection("another section"));
    QVERIFY(!conf->hasSection("section"));
    QVERIFY(conf->deleteSection("another section"));
    QVERIFY(conf->save("config.test"));
}

void TestConfig::cleanupTestCase()
{
    delete conf;
}
QTEST_MAIN(TestConfig)
#include "testconfig.moc"
