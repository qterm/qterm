#ifndef QTERMPARAM_H
#define QTERMPARAM_H

// #include <qstring.h>
// #include <qcolor.h>

#include <QString>
#include <QColor>
#include <QVariant>

namespace QTerm
{
class Param;

class Param
{

public:
	Param();
	Param( const Param& );
	~Param();

	Param& operator=(const Param&);

	QVariantMap m_mapParam;
};

} // namespace QTerm

#endif // QTERMPARAM_H
