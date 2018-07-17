#ifndef SOUNDCONF_H
#define SOUNDCONF_H

#include <QButtonGroup>
#include "ui_soundconf.h"
namespace QTerm
{
class Sound;

class soundConf : public QDialog
{
	Q_OBJECT

public:
	soundConf( QWidget* parent = 0, Qt::WindowFlags fl = 0 );
	~soundConf();
	void loadSetting();
	void saveSetting();

public slots:
	void onSelectFile();
	void onSelectProg();
	void onPlayMethod( int id );
	void onTestPlay();
protected slots:
	void accept();

private:
	Sound * m_pSound;
	Ui::soundConf ui;
	QButtonGroup bgMethod;
};

} // namespace QTerm

#endif // SOUNDCONF_H
