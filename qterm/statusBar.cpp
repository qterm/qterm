#include "overlayWidget.h"
#include "statusBar.h"

#include <qapplication.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qobjectlist.h> //polish()
#include <qpainter.h>
#include <qpalette.h>
#include <qprogressbar.h>
#include <qstyle.h>   //class CloseButton
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qtooltip.h> //QToolTip::palette()
#include <qvbox.h>
#include <qpixmap.h>
#include <qiconset.h>
//segregated classes
#include "popupMessage.h"
#include "progressBar.h"

extern QString pathPic;

namespace QTerm
{
	
//TODO allow for uncertain progress periods

StatusBar* StatusBar::s_instance = 0;

StatusBar::StatusBar( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    s_instance = this;
    QBoxLayout *mainlayout = new QHBoxLayout( this, 2, /*spacing*/5 );

    //we need extra spacing due to the way we paint the surrounding boxes
    QBoxLayout *layout = new QHBoxLayout( mainlayout, /*spacing*/5 );

    m_mainTextLabel = new QLabel( this, "mainTextLabel" );

    QHBox *mainProgressBarBox = new QHBox( this, "progressBox" );
    QToolButton *b1 = new QToolButton( mainProgressBarBox, "cancelButton" );
    m_mainProgressBar  = new QProgressBar( mainProgressBarBox, "mainProgressBar" );
    QToolButton *b2 = new QToolButton( mainProgressBarBox, "showAllProgressDetails" );
    mainProgressBarBox->setSpacing( 2 );
    mainProgressBarBox->hide();

    layout->add( m_mainTextLabel );
    layout->add( mainProgressBarBox );
    layout->setStretchFactor( m_mainTextLabel, 3 );
    layout->setStretchFactor( mainProgressBarBox, 1 );

    m_otherWidgetLayout = new QHBoxLayout( mainlayout, /*spacing*/5 );

    mainlayout->setStretchFactor( layout, 6 );
    mainlayout->setStretchFactor( m_otherWidgetLayout, 4 );

    //b1->setIconSet( SmallIconSet( "cancel" ) );
    //b2->setIconSet( SmallIconSet( "2uparrow") );
    b1->setIconSet( QPixmap( pathPic + "pic/messagebox_critical.png" ));
    b2->setIconSet( QPixmap( pathPic + "pic/messagebox_info.png" ));
    b2->setToggleButton( true );
    QToolTip::add( b1, tr( "Abort all background-operations" ) );
    QToolTip::add( b2, tr( "Show progress detail" ) );
    connect( b1, SIGNAL(clicked()), SLOT(abortAllProgressOperations()) );
    connect( b2, SIGNAL(toggled( bool )), SLOT(toggleProgressWindow( bool )) );

    m_popupProgress = new PopupMessage( this, mainProgressBarBox, 0, "popupProgress" );
    m_popupProgress->showCloseButton( false );
    m_popupProgress->showCounter( false );
    m_popupProgress->setMargin( 1 );
    m_popupProgress->setFrameStyle( QFrame::Box | QFrame::Raised );
    m_popupProgress->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}

void
StatusBar::addWidget( QWidget *widget )
{
    m_otherWidgetLayout->add( widget );
}


/// reimplemented functions

void
StatusBar::polish()
{
    QWidget::polish();

    int h = 0;
    QObjectList *list = queryList( "QWidget", 0, false, false );

	QObject * o;

    for( o = list->first(); o; o = list->next() ) {
        int _h = static_cast<QWidget*>( o ) ->minimumSizeHint().height();
        if ( _h > h )
            h = _h;

//        debug() << o->className() << ", " << o->name() << ": " << _h << ": " << static_cast<QWidget*>(o)->minimumHeight() << endl;

        if ( o->inherits( "QLabel" ) )
            static_cast<QLabel*>(o)->setIndent( 4 );
    }

    h -= 4; // it's too big usually

    for ( o = list->first(); o; o = list->next() )
        static_cast<QWidget*>(o)->setFixedHeight( h );

    delete list;
}

void
StatusBar::paintEvent( QPaintEvent* )
{
    QObjectList *list = queryList( "QWidget", 0, false, false );
    QPainter p( this );

    for( QObject * o = list->first(); o; o = list->next() ) {
        QWidget *w = (QWidget*)o;

        if ( !w->isVisible() )
            continue;

        style().drawPrimitive(
                QStyle::PE_StatusBarSection,
                &p,
                QRect( w->x() - 1, w->y() - 1, w->width() + 2, w->height() + 2 ),
                colorGroup(),
                QStyle::Style_Default,
                QStyleOption( w ) );
    }

    delete list;
}

bool
StatusBar::event( QEvent *e )
{
    if ( e->type() == QEvent::LayoutHint )
        update();

    return QWidget::event( e );
}


/// Messaging system

void
StatusBar::setMainText( const QString &text )
{
    m_mainText = text;

    // it may not be appropriate for us to set the mainText yet
    resetMainText();
}

void
StatusBar::shortMessage( const QString &text )
{
    m_mainTextLabel->setText( text );
    m_mainTextLabel->setPalette( QToolTip::palette() );

    //SingleShotPool::startTimer( 5000, this, SLOT(resetMainText()) );
    resetMainText();
}

void
StatusBar::resetMainText()
{
	/*
    if( sender() )
        debug() << sender()->name() << endl;

    // don't reset if we are showing a shortMessage
    if( SingleShotPool::isActive( this, SLOT(resetMainText()) ) )
        return;
*/
    m_mainTextLabel->unsetPalette();

    if( allDone() )
        m_mainTextLabel->setText( m_mainText );

    else {
        m_mainTextLabel->setPaletteBackgroundColor( m_mainTextLabel->paletteBackgroundColor().dark( 110 ) );

        ProgressBar *bar = 0;
        uint count = 0;
        foreachType( ProgressMap, m_progressMap )
            if( !(*it)->m_done ) {
                bar = *it;
                count++;
            }

        if( count == 1 )
            m_mainTextLabel->setText( bar->description() + "..." );
        else
            m_mainTextLabel->setText( tr("Multiple background-tasks running") );
    }
}
/*
void
StatusBar::shortLongMessage( const QString &_short, const QString &_long )
{
    shortMessage( _short );

    if ( !_long.isEmpty() ) {
        AMAROK_NOTIMPLEMENTED
    }
}
*/
//void
//StatusBar::longMessage( const QString &text, int /*type*/ )
/*{
    PopupMessage * message;
    message = new PopupMessage( this, m_mainTextLabel );
    message->setText( text );

    if ( !m_messageQueue.isEmpty() )
         message->stackUnder( m_messageQueue.last() );

    message->reposition();
    message->display();

    raise();

    m_messageQueue += message;
}
*/

//void
//StatusBar::longMessageThreadSafe( const QString &text, int /*type*/ )
/*{
    QCustomEvent * e = new QCustomEvent( 1000 );
    e->setData( new QString( text ) );
    QApplication::postEvent( this, e );
}
*/
void
StatusBar::customEvent( QCustomEvent *e )
{
    QString *s = static_cast<QString*>( e->data() );
    shortMessage( *s );
    delete s;
}


/// application wide progress monitor

inline bool
StatusBar::allDone()
{
    for( ProgressMap::Iterator it = m_progressMap.begin(), end = m_progressMap.end(); it != end; ++it )
        if( (*it)->m_done == false )
            return false;

    return true;
}

ProgressBar&
StatusBar::newProgressOperation( QObject *owner)
{
    if ( m_progressMap.contains( owner ) )
        return *m_progressMap[owner];

    if(m_progressMap.count()==0)
	    emit started();

    if( allDone() )
        // if we're allDone then we need to remove the old progressBars before
        // we start anything new or the total progress will not be accurate
        pruneProgressBars();
    else
        static_cast<QWidget*>(progressBox()->child("showAllProgressDetails"))->show();


    QHBox * hbox = new QHBox( m_popupProgress );
    QLabel *label = new QLabel( hbox );
    ProgressBar *pBar = new ProgressBar( hbox, label );
    hbox->show();
    m_popupProgress->addWidget( hbox );
    m_progressMap.insert( owner, pBar );


    connect( owner, SIGNAL(destroyed( QObject* )), SLOT(endProgressOperation( QObject* )) );

    // so we can show the correct progress information
    // after the ProgressBar is setup
    //SingleShotPool::startTimer( 0, this, SLOT(updateProgressAppearance()) );
    updateProgressAppearance();

    progressBox()->show();
    cancelButton()->setEnabled( true );

    return *m_progressMap[ owner ];
}
/*
ProgressBar&
StatusBar::newProgressOperation( KIO::Job *job )
{
    ProgressBar & bar = newProgressOperation( (QObject*)job );
    bar.setTotalSteps( 100 );

    if(!allDone())
        static_cast<QWidget*>(progressBox()->child("showAllProgressDetails"))->show();
    connect( job, SIGNAL(result( KIO::Job* )), SLOT(endProgressOperation()) );
    //TODO connect( job, SIGNAL(infoMessage( KIO::Job *job, const QString& )), SLOT() );
    connect( job, SIGNAL(percent( KIO::Job*, unsigned long )), SLOT(setProgress( KIO::Job*, unsigned long )) );

    return bar;
}
*/
/*
void
StatusBar::endProgressOperation()
{
    QObject *owner = (QObject*)sender(); //HACK deconsting it
    KIO::Job *job = dynamic_cast<KIO::Job*>( owner );

    //FIXME doesn't seem to work for KIO::DeleteJob, it has it's own error handler and returns no error too
    // if you try to delete http urls for instance <- KDE SUCKS!

    if( job && job->error() )
        longMessage( job->errorString(), Error );

    endProgressOperation( owner );
}
*/
void
StatusBar::endProgressOperation( QObject *owner )
{
    //the owner of this progress operation has been deleted
    //we need to stop listening for progress from it
    //NOTE we don't delete it yet, as this upsets some
    //things, we just call setDone().

    if ( !m_progressMap.contains( owner ) )
        return ;

    m_progressMap[owner]->setDone();

    if( allDone() && !m_popupProgress->isShown() ) {
        cancelButton()->setEnabled( false );
        //SingleShotPool::startTimer( 2000, this, SLOT(hideMainProgressBar()) );
	hideMainProgressBar();
    }

    updateTotalProgress();

    if(m_progressMap.count()==0)
	emit ended();
}

void
StatusBar::abortAllProgressOperations() //slot
{
    for( ProgressMap::Iterator it = m_progressMap.begin(), end = m_progressMap.end(); it != end; ++it )
        (*it)->m_abort->animateClick();

    m_mainTextLabel->setText( tr("Aborting all jobs...") );

    cancelButton()->setEnabled( false );
}

void
StatusBar::toggleProgressWindow( bool show ) //slot
{
    m_popupProgress->adjustSize(); //FIXME shouldn't be needed, adding bars doesn't seem to do this
    m_popupProgress->setShown( show );

    if( !show )
	hideMainProgressBar();
        //SingleShotPool::startTimer( 2000, this, SLOT(hideMainProgressBar()) );
}

void
StatusBar::showMainProgressBar()
{
    if( !allDone() )
        progressBox()->show();
}

void
StatusBar::hideMainProgressBar()
{
    if( allDone() && !m_popupProgress->isShown() )
    {
        pruneProgressBars();

        resetMainText();

        m_mainProgressBar->setProgress( 0 );
        progressBox()->close();
    }
}

void
StatusBar::setProgress( int steps )
{
    setProgress( sender(), steps );
}
/*
void
StatusBar::setProgress( KIO::Job *job, unsigned long percent )
{
    setProgress( ( QObject* ) job, percent );
}
*/
void
StatusBar::setProgress( const QObject *owner, int steps )
{
    if ( !m_progressMap.contains( owner ) )
        return ;

    m_progressMap[ owner ] ->setProgress( steps );

    updateTotalProgress();
}

void
StatusBar::setProgressStatus( const QObject *owner, const QString &text )
{
    if ( !m_progressMap.contains( owner ) )
        return ;

    m_progressMap[owner]->setStatus( text );
}

void StatusBar::incrementProgress()
{
    incrementProgress( sender() );
}

void
StatusBar::incrementProgress( const QObject *owner )
{
    if ( !m_progressMap.contains( owner ) )
        return;

    m_progressMap[owner]->setProgress( m_progressMap[ owner ] ->progress() + 1 );

    updateTotalProgress();
}

void
StatusBar::updateTotalProgress()
{
    uint totalSteps = 0;
    uint progress = 0;

    foreachType( ProgressMap, m_progressMap ) {
        totalSteps += (*it)->totalSteps();
        progress += (*it)->progress();
    }

    if( totalSteps == 0 && progress == 0 )
        return;

    m_mainProgressBar->setTotalSteps( totalSteps );
    m_mainProgressBar->setProgress( progress );

    pruneProgressBars();
}

void
StatusBar::updateProgressAppearance()
{
    toggleProgressWindowButton()->setShown( m_progressMap.count() > 1 );

    resetMainText();

    updateTotalProgress();
}

void
StatusBar::pruneProgressBars()
{
    ProgressMap::Iterator it = m_progressMap.begin();
    const ProgressMap::Iterator end = m_progressMap.end();
    int count = 0;
    bool removedBar = false;
    while( it != end )
        if( (*it)->m_done == true ) {
            delete (*it)->m_label;
            delete (*it)->m_abort;
            delete (*it);

            ProgressMap::Iterator jt = it;
            ++it;
            m_progressMap.erase( jt );
            removedBar = true;
        }
        else {
            ++it;
            ++count;
        }
    if(count==1 && removedBar) //if its gone from 2 or more bars to one bar...
    {
        resetMainText();
        static_cast<QWidget*>(progressBox()->child("showAllProgressDetails"))->hide();
        m_popupProgress->setShown(false);
    }
}

} //namespace QTerm

#ifdef HAVE_CONFIG_H
#include "statusBar.moc"
#endif
