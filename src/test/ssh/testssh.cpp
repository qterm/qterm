#include "testssh.h"
#include "hostinfo.h"
#include "ssh/socket.h"
using namespace QTerm;

void TestSSH::testSSH()
{
    SSHSocket * socket = new SSHSocket;
    SSHInfo * hostInfo = new SSHInfo("test", 22);
    hostInfo->setUserName("hooey");
    socket->connectToHost(hostInfo);
    QTest::qWait(2000);
    //QVERIFY(socket->status() == "OK");
}


QTEST_MAIN(TestSSH)
#include "testssh.moc"
