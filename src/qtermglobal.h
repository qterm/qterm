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
#include <QtCore/QMap>
#include <QtCore/QObject>

namespace QTerm
{
class Config;
class Convert;
class Global : public QObject
{
    Q_OBJECT
public:
    enum Language {
        SimpilifiedChinese,
        TraditionalChinese,
        English
    };
    enum Codec {
        GBK,
        Big5
    };
    enum Status {
        INIT_OK,
        INIT_ERROR
    };
    enum Conversion {
        No_Conversion = 0,
        Simplified_To_Traditional,
        Traditional_To_Simplified
    };
    struct Pref {
        Codec  nXIM;
        int  nWordWrap;
        bool bWheel;
        bool bWarn;
        bool bBlinkTab;
        QString strHttp;
        int  nBeep;
        QString strWave;
        QString strPlayer;
        bool bUrl;
        bool bAutoCopy;
        bool bAA;
        bool bTray;
        bool bClearPool;
        QString strZmPath;
        QString strPoolPath;
        QString strImageViewer;
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
    Pref m_pref;
    void loadPrefence();
    enum Position {Hide, Left, Right};
    bool isBossColor() const;
    const QString & escapeString() const;
    Conversion clipConversion() const;
    Language language() const;
    bool showStatusBar() const;
    Position scrollPosition() const;
    bool isFullScreen() const;
    bool showSwitchBar() const;
    bool showToolBar(const QString & toolbar);
    const QString & style() const;

    void setClipConversion(Conversion conversionId);
    void setEscapeString(const QString & escapeString);
    void setScrollPosition(Position position);
    void setStatusBar(bool isShow); //Better name?
    void setBossColor(bool isBossColor);
    void setFullScreen(bool isFullscreen);
    void setSwitchBar(bool isShow);
    void setLanguage(const Language language);
    void setStyle(const QString & style);
    void setShowToolBar(const QString & toolbar, bool isShown);
    void saveShowToolBar();
    void loadConfig(); //TODO: Merge with iniSettings
    void saveConfig();
    QByteArray loadGeometry();
    QByteArray loadState();
    void saveGeometry( const QByteArray geometry);
    void saveState( const QByteArray state);
    void cleanup();
    void openUrl(const QString & url);
    QString convert(const QString & source, Conversion flag);

private:
    Global();
    static Global* m_Instance;
    bool iniWorkingDir(QString param);
    bool iniSettings();
    bool isPathExist(const QString & path);
    bool createLocalFile(const QString & dst, const QString & src);
    void closeNotification(uint id);
    QString m_fileCfg;
    QString m_addrCfg;
    QString m_pathLib;
    QString m_pathPic;
    QString m_pathCfg;
    Config * m_config;
    Config * m_address;
    QByteArray * m_windowState;
    Status m_status;
    QString m_style;
    bool m_bossColor;
    QString m_escape;
    Conversion m_clipConversion;
    Convert * m_converter;
    bool m_statusBar;
    Position m_scrollPos;
    bool m_fullScreen;
    bool m_switchBar;
    Language m_language;
    QMap<QString, bool> m_showToolBar;
};

} // namespace QTerm
#endif // GLOBAL_H
