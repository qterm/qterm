#ifndef FSSHLOGIN_H
#define FSSHLOGIN_H
#include "ui/ui_sshlogin.h"

class SSHLogin : public QDialog
{
    Q_OBJECT

public:
    SSHLogin(QString * username, QString * password,  QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~SSHLogin();
private:
    QString * strUserName;
    QString * strPassword;
	Ui::SSHLogin ui;
private slots:
    void accept();
};

#endif // FSSHLOGIN_H
