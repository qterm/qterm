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

#include <QtCore/QTranslator>
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtXml/QDomDocument>

namespace QTerm
{
class Config;
class Convert;
#ifdef KWALLET_ENABLED
class Wallet;
#endif //KWALLET_ENABLED
class Global : public QObject
{
    Q_OBJECT
public:
    enum Language {
        SimplifiedChinese,
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

    enum Position {
        Hide = 0, 
        Left, 
        Right
    };

    struct Pref {
        Conversion XIM;
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
    QDomDocument addrXml();
    const QString & pathLib();
    const QString & pathPic();
    const QString & pathCfg();
    void clearDir(const QString & path);
    QMap<QString,QString> loadFavoriteList(QDomDocument);
    QStringList loadNameList();
    bool loadAddress(QDomDocument doc, QString uuid, Param & param);
    bool loadAddress(int n, Param & param);
    void saveAddress(int n, const Param & param);
    void saveAddress(QDomDocument doc, QString uuid, const Param & param);
    void removeAddress(int n);
    void removeAddress(QDomDocument doc, QString uuid);
    void saveAddressXml(const QDomDocument& doc);
    bool convertAddressBook2XML();
    QString getOpenFileName(const QString & filter, QWidget * widget);
    QString getSaveFileName(const QString & filename, QWidget * widget);
    bool isOK();
    Pref m_pref;
    void loadPrefence();
    bool isBossColor() const;
    const QString & escapeString() const;
    Conversion clipConversion() const;
    Language language() const;
    Position scrollPosition() const;
    bool isFullScreen() const;
    bool showSwitchBar() const;
    bool showStatusBar() const;
    bool showMenuBar() const;

    const QString & style() const;

    void setClipConversion(Conversion conversionId);
    void setEscapeString(const QString & escapeString);
    void setScrollPosition(Position position);
    void setStatusBar(bool isShow); //Better name?
    void setMenuBar(bool isShow);
    void setBossColor(bool isBossColor);
    void setFullScreen(bool isFullscreen);
    void setSwitchBar(bool isShow);
    void setLanguage(const Language language);
    void setStyle(const QString & style);
    void loadConfig(); //TODO: Merge with iniSettings
    void saveConfig();
    QByteArray loadGeometry();
    QByteArray loadState();
    void saveGeometry( const QByteArray geometry);
    void saveState( const QByteArray state);
    void saveSession(const QList<QVariant>& sites);
    QList<QVariant> loadSession();
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
    QString m_addrXml;
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
    bool m_menuBar;
    Language m_language;
    QTranslator *m_translatorQT;
    QTranslator *m_translatorQTerm;
#ifdef KWALLET_ENABLED
    Wallet * m_wallet;
#endif // KWALLET_ENABLED
};

} // namespace QTerm
#endif // GLOBAL_H
