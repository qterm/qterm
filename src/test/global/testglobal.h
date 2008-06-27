#ifndef TEST_GLOBAL_H
#define TEST_GLOBAL_H
#include <QtTest>

namespace QTerm
{
class TestGlobal : public QObject
{
    Q_OBJECT
private slots:
    void testGlobal();
};

} // namespace QTerm
#endif // TEST_GLOBAL_H

