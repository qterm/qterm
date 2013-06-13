#include "testhostinfo.h"
#include "hostinfo.h"
#include <QtDebug>

using namespace QTerm;

void TestHostInfo::testHostInfo()
{
    HostInfo hostinfo("newsmth.net", 22);
    QVERIFY(hostinfo.hostName() == "newsmth.net");
}

void TestHostInfo::testSSHHostInfo()
{
    SSHInfo hostinfo("newsmth.net", 22);
    hostinfo.setUserName("hooey");
    Completion completion;
    completion["who"]="Lu Ran";
    completion["test"]="Temp";
    hostinfo.setAutoCompletion(completion);
    bool test=false;
    QVERIFY(hostinfo.userName(&test) == "hooey");
    QVERIFY(test);
    QVERIFY(hostinfo.answer("test") == "Temp");
    QVERIFY(hostinfo.userName(&test) == "hooey");
}
QTEST_MAIN(TestHostInfo)
#include "testhostinfo.moc"
