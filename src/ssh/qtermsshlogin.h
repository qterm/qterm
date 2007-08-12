#ifndef FSSHLOGIN_H
#define FSSHLOGIN_H
#include "ui_sshlogin.h"

namespace QTerm
{
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

} // namespace QTerm

#endif // FSSHLOGIN_H
