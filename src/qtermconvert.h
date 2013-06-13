#ifndef QTERMCONVERT_H
#define QTERMCONVERT_H
#include <QtCore/QString>
class QTextCodec;
namespace QTerm
{
class Convert
{
public:
	Convert();
	~Convert();

	QString S2T(const QString & source);
	QString T2S(const QString & source);

private:
	char * G2B( const char * string, int length );
	char * B2G( const char * string, int length );
	void g2b( unsigned char c1, unsigned char c2, char * s);
	void b2g( unsigned char c1, unsigned char c2, char * s);

	static unsigned char GtoB[];
	static unsigned char BtoG[];
	QTextCodec * m_gbk;
	QTextCodec * m_big5;
};

} // namespace QTErm

#endif //QTERMCONVERT_H
