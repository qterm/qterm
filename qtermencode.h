#ifndef QTERMENCODE_H
#define QTERMENCODE_H

#include <QObject>

class QTermBuffer;

class QTermEncode : public QObject
{
	Q_OBJECT
	
public:
	QTermEncode( QTermBuffer * );
	~QTermEncode();

	QByteArray encode(const QByteArray escape="");
	
private:		
	QTermBuffer * m_pBuffer; // the buffer operated on

};
	
#endif // QTERMENCODE_H
