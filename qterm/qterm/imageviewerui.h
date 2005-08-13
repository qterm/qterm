/****************************************************************************
** Form interface generated from reading ui file 'imageviewerui.ui'
**
** Created: 五  8月 12 14:26:46 2005
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef IMAGEVIEWERUI_H
#define IMAGEVIEWERUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QListView;
class QListViewItem;
class QPushButton;

class ImageViewerUI : public QDialog
{
    Q_OBJECT

public:
    ImageViewerUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ImageViewerUI();

    QListView* imageList;
    QPushButton* buttonClose;

protected:
    QVBoxLayout* ImageViewerUILayout;
    QHBoxLayout* layout13;
    QSpacerItem* spacer3;
    QSpacerItem* spacer4;

protected slots:
    virtual void languageChange();

};

#endif // IMAGEVIEWERUI_H
