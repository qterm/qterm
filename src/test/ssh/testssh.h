#ifndef SSH_TEST_H
#define SSH_TEST_H
#include <QtTest>

namespace QTerm
{

class TestSSH: public QObject
{
    Q_OBJECT
private slots:
    void testSSH();
};

} // namespace QTerm
#endif
