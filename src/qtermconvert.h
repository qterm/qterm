#ifndef QTERMCONVERT_H
#define QTERMCONVERT_H
namespace QTerm
{
class Convert
{
public:
	Convert();
	~Convert();

	char * G2B( const char * string, int length );
	char * B2G( const char * string, int length );

private:
	void g2b( unsigned char c1, unsigned char c2, char * s);
	void b2g( unsigned char c1, unsigned char c2, char * s);

	static unsigned char GtoB[];
	static unsigned char BtoG[];
};

} // namespace QTErm

#endif //QTERMCONVERT_H
