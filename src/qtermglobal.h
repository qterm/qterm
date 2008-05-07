//
// C++ Interface: Global
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QTERMGLOBAL_H
#define QTERMGLOBAL_H

#include "qtermparam.h"

#include <QtCore/QMutex>
#include <QtCore/QString>

namespace QTerm
{
class Config;
class Global
{
public:
    enum Status {
        OK,
        ERROR
    };
    static Global * instance();
    Config * fileCfg();
    Config * addrCfg();
    const QString & pathLib();
    const QString & pathPic();
    const QString & pathCfg();
    void clearDir(const QString & path);
    QStringList loadNameList();
    bool loadAddress(int n, Param & param);
    void saveAddress(int n, const Param & param);
    QString getOpenFileName(const QString & filter, QWidget * widget);
    QString getSaveFileName(const QString & filename, QWidget * widget);
    bool isOK();

private:
    Global();
    static Global* m_Instance;
    bool iniWorkingDir(QString param);
    bool iniSettings();
    bool isPathExist(const QString & path);
    bool createLocalFile(const QString & dst, const QString & src);
    QString m_fileCfg;
    QString m_addrCfg;
    QString m_pathLib;
    QString m_pathPic;
    QString m_pathCfg;
    Config * m_config;
    Config * m_address;
    Status m_status;

};

} // namespace QTerm
#endif // GLOBAL_H
