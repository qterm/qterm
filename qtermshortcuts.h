#ifdef QTERMSHORTCUTS_H

#include <QString>
#include <QMap>

class QTermShortcuts
{
public:
	QTermShortcuts();
	~QTermShortcuts();
	
	void load(const QString&);
	void save(const QString&);

private:
	QMap<QString, QAccel> conf;

};

#endif //QTERMSHORTCUTS_H
