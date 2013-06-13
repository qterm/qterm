#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H
#include <QtTest>

namespace QTerm
{
class Config;
class TestConfig : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void test();
    void cleanupTestCase();
private:
    Config * conf;
};

} // namespace QTerm
#endif

