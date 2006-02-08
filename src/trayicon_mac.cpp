#include "trayicon.h"
#include <qglobal.h>

#ifdef Q_OS_MACX 
void TrayIcon::sysInstall()
{
}

void TrayIcon::sysRemove()
{
}

void TrayIcon::sysUpdateIcon()
{
}

void TrayIcon::sysUpdateToolTip()
{
}
#endif // Q_OS_MACX 
