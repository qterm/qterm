#include "qtermcanvas.h"
#include "qterm.h"
#include "qtermconfig.h"
#include "qtermglobal.h"

#include <QCloseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QMatrix>

namespace QTerm
{
// extern QString getSaveFileName(const QString&, QWidget*);

Canvas::Canvas(QWidget *parent, Qt::WFlags f)
        : QScrollArea(parent)
{

    setWindowFlags(f);
    //dirty trick
    if (f == 0)
        bEmbed = true;
    else
        bEmbed = false;

    m_pMenu = new QMenu(this);
    addAction(m_pMenu->addAction(tr("Original Size"), this, SLOT(oriSize()), Qt::Key_Z));
    m_fitAction = m_pMenu->addAction(tr("Fit Window"));
    m_fitAction->setCheckable(true);
    m_fitAction->setShortcut(Qt::Key_X);
    m_fitAction->setChecked(true);
    connect(m_fitAction, SIGNAL(toggled(bool)), this, SLOT(fitWin(bool)));
    addAction(m_fitAction);
    m_pMenu->addSeparator();
    addAction(m_pMenu->addAction(tr("Zoom In"), this, SLOT(zoomIn()), Qt::Key_Equal));
    addAction(m_pMenu->addAction(tr("Zoom Out"), this, SLOT(zoomOut()), Qt::Key_Minus));
    if (!bEmbed)
        addAction(m_pMenu->addAction(tr("Fullscreen"), this, SLOT(fullScreen()), Qt::Key_F));
    m_pMenu->addSeparator();
    addAction(m_pMenu->addAction(tr("Rotate CW 90"), this, SLOT(cwRotate()), Qt::Key_BracketRight));
    addAction(m_pMenu->addAction(tr("Rotate CCW 90"), this, SLOT(ccwRotate()), Qt::Key_BracketLeft));

    if (!bEmbed) {
        m_pMenu->addSeparator();
        addAction(m_pMenu->addAction(tr("Save As..."), this, SLOT(saveImage()), Qt::Key_S));
        addAction(m_pMenu->addAction(tr("Copy To..."), this, SLOT(copyImage()), Qt::Key_C));
        addAction(m_pMenu->addAction(tr("Silent Copy"), this, SLOT(silentCopy()), Qt::Key_S + Qt::SHIFT));
        addAction(m_pMenu->addAction(tr("Delete"), this, SLOT(deleteImage()), Qt::Key_D));

        m_pMenu->addSeparator();
        addAction(m_pMenu->addAction(tr("Exit"), this, SLOT(close()), Qt::Key_Q));
    }

    bFitWin = true;

    label = new QLabel(viewport());
    label->setScaledContents(true);
    label->setAlignment(Qt::AlignCenter);
    label->setText("No Preview Available");
    setWidget(label);
    resize(200, 100);
    setAlignment(Qt::AlignCenter);

}
Canvas::~Canvas()
{
    //delete label;
    //delete m_pMenu;
}

void Canvas::oriSize()
{
    m_fitAction->setChecked(false);
    szImage = img.size();
    resize(img.size()+QSize(frameWidth()*2,frameWidth()*2));
    adjustSize(viewport()->size());
}

void Canvas::zoomIn()
{
    m_fitAction->setChecked(false);
    resizeImage(0.05);
}

void Canvas::zoomOut()
{
    m_fitAction->setChecked(false);
    resizeImage(-0.05);
}

void Canvas::fitWin(bool checked)
{
    bFitWin = checked;
    if (bFitWin)
        adjustSize(maximumViewportSize());
}

void Canvas::cwRotate()
{
    rotateImage(90);
}

void Canvas::ccwRotate()
{
    rotateImage(-90);
}

void Canvas::fullScreen()
{
    if (!isFullScreen())
        showFullScreen();
    else
        showNormal();
}

void Canvas::loadImage(QString name)
{
    img.load(name);
    if (!img.isNull()) {

        strFileName = name;
        setWindowTitle(QFileInfo(name).fileName());

        QSize szView(img.size());
        szView.scale(1024, 768, Qt::KeepAspectRatio);

        if (szView.width() < img.width()) {
            szImage = szView;
            label->resize(szImage);
            label->setPixmap(scaleImage(szImage));
            if (!bEmbed)
                resize(szView+QSize(frameWidth()*2,frameWidth()*2));
        } else {
            szImage = img.size();
            label->resize(szImage);
            label->setPixmap(QPixmap::fromImage(img));
            if (!bEmbed)
                resize(szImage+QSize(frameWidth()*2,frameWidth()*2));
        }
        if (bEmbed)
            fitWin(true);

    } else
        qWarning("cant load image");
}

void Canvas::resizeImage(float ratio)
{
    if (label->pixmap() == NULL)
        return;

    QSize szImg = szImage;
    szImg *= (1 + ratio);
    //we dont need so big
    if (szImg.width() > 10000 || szImg.height() > 10000)
        return;
    szImage = szImg;

    if (!isFullScreen() && !bEmbed)
        resize(szImage+QSize(frameWidth()*2,frameWidth()*2));
    else
        adjustSize(viewport()->size());
}

void Canvas::rotateImage(float ang)
{
    QMatrix wm;

    wm.rotate(ang);

    img = img.transformed(wm);

    szImage = img.size();

    adjustSize(viewport()->size());
}

void Canvas::copyImage()
{
    QFileInfo fi(strFileName);
    QString strSave = QFileDialog::getSaveFileName(this, "Choose a filename to save under", QDir::currentPath() + fi.fileName());

    if (strSave.isEmpty())
        return;
    QFile file(strFileName);
    if (file.open(QIODevice::ReadOnly)) {
        QFile save(strSave);
        if (save.open(QIODevice::WriteOnly)) {
            QByteArray ba = file.readAll();
            QDataStream ds(&save);
            ds.writeRawData(ba, ba.size());
            save.close();
        }
        file.close();
    }
}

void Canvas::silentCopy()
{
    // save it to $savefiledialog
    QString strPath = Global::instance()->fileCfg()->getItemValue("global", "savefiledialog").toString();

    QFileInfo fi(strFileName);
    QString strSave = strPath + "/" + fi.fileName();

    fi.setFile(strSave);

    // add (%d) if exist
    int i = 1;
    while (fi.exists()) {
        strSave = QString("%1/%2(%3).%4")
                  .arg(fi.dir().path())
                  .arg(fi.completeBaseName())
                  .arg(i)
                  .arg(fi.suffix());
        fi.setFile(strSave);
    }

    // copy it
    QFile file(strFileName);
    if (file.open(QIODevice::ReadOnly)) {
        QFile save(strSave);
        if (save.open(QIODevice::WriteOnly)) {
            QByteArray ba = file.readAll();
            QDataStream ds(&save);
            ds.writeRawData(ba, ba.size());
            save.close();
        }
        file.close();
    }
}


QPixmap Canvas::scaleImage(const QSize& sz)
{
    return QPixmap::fromImage(img.scaled(sz));
}

void Canvas::moveImage(float dx, float dy)
{
    scrollContentsBy(widget()->width()*dx, widget()->height()*dy);
}

void Canvas::saveImage()
{
    QFileInfo fi(strFileName);
    QString strSave = QFileDialog::getSaveFileName(this, tr("Choose a filename to save under"), QDir::currentPath() + fi.fileName());

    if (strSave.isEmpty())
        return;
    QString fmt = fi.suffix().toUpper();
    if (!img.save(strSave, fmt.toLatin1()))
        QMessageBox::warning(this, tr("Failed to save file"), tr("Cant save file, maybe format not supported"));
}

void Canvas::deleteImage()
{
    QFile::remove(strFileName);
    close();
}

void Canvas::closeEvent(QCloseEvent *ce)
{
    if (!bEmbed)
        delete this;
}

void Canvas::resizeEvent(QResizeEvent *re)
{
    adjustSize(re->size());
}

void Canvas::mousePressEvent(QMouseEvent *me)
{
    if(me->button()&Qt::LeftButton)
    {
        close();
        return;
    }
    if (me->button()&Qt::RightButton) {
        m_pMenu->popup(me->globalPos());
    }
}

void Canvas::keyPressEvent(QKeyEvent *ke)
{
    switch (ke->key()) {
    case Qt::Key_Escape:
        if (isFullScreen())
            showNormal();
        else
            close();
        break;
    case Qt::Key_Left:
        moveImage(-0.05, 0);
        break;
    case Qt::Key_Right:
        moveImage(0.05, 0);
        break;
    case Qt::Key_Up:
        moveImage(0, -0.05);
        break;
    case Qt::Key_Down:
        moveImage(0, 0.05);
        break;

    }
}

void Canvas::adjustSize(const QSize& szView)
{
    if (label->pixmap() == NULL) {
        label->resize(width(), height());
        return;
    }

    QSize szImg = szImage;

    if (bFitWin) {
        if (szImg.width() > szView.width() ||
                szImg.height() > szView.height() ||
                szImg.width() < img.width()) {
            szImg.scale(szView, Qt::KeepAspectRatio);
        }
        szImg = szImg.boundedTo(img.size());
    }

    szImage = szImg;

    label->resize(szImage);

    label->setPixmap(scaleImage(szImage));

}

} // namespace QTerm

#include <qtermcanvas.moc>
