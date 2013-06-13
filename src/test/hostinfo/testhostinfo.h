#ifndef TEST_HOSTINFO_H
#define TEST_HOSTINFO_H

#include <QtTest>

namespace QTerm
{
class TestHostInfo : public QObject
{
    Q_OBJECT
private slots:
    void testHostInfo();
    void testSSHHostInfo();
};
} // namespace QTerm
#endif // TEST_HOSTINFO_H
