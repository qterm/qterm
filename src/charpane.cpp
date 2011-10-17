#include "charpane.h"
#include "chartable.h"

#include <QLabel>
#include <QToolButton>
#include <QBoxLayout>
#include <QMapIterator>

namespace QTerm {
Header::Header(const QString &title, QWidget *parent)
    : QFrame(parent), m_bOpen(true)
{
    setFrameShape(QFrame::StyledPanel);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    QLabel *label = new QLabel(title);
    layout->addWidget(label);
    layout->addSpacing(1);
    
    m_buttonOpen = new QToolButton();
    m_buttonOpen->setArrowType(Qt::UpArrow);
    layout->addWidget(m_buttonOpen);
    
    setLayout(layout);

    connect(m_buttonOpen, SIGNAL(clicked()), SLOT(toggleOpen()));
}

void Header::toggleOpen()
{
    m_bOpen = !m_bOpen;
    if (m_bOpen)
        m_buttonOpen->setArrowType(Qt::UpArrow);
    else
        m_buttonOpen->setArrowType(Qt::DownArrow);
    emit toggled(m_bOpen);
}

CharPane::CharPane(QWidget *parent)
    : QWidget(parent)
{
    m_layoutVBox = new QVBoxLayout();
    m_layoutVBox->addStretch();
    setLayout(m_layoutVBox);

    mapSymbol["Number"] = QString::fromUtf8(
                "⒈⒉⒊⒋⒌⒍⒎⒏⒐⒑"
                "⒒⒓⒔⒕⒖⒗⒘⒙⒚⒛"
                "①②③④⑤⑥⑦⑧⑨⑩"
                "➀➁➂➃➄➅➆➇➈➉"
                "❶❷❸❹❺❻❼❽❾❿"
                "➊➋➌➍➎➏➐➑➒➓"
                "⑴⑵⑶⑷⑸⑹⑺⑻⑼⑽"
                "㈠㈡㈢㈣㈤㈥㈦㈧㈨㈩"
                "ⅰⅱⅲⅳⅴⅵⅶⅷⅸⅹ"
                "ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");

    mapSymbol["Arrow"] = QString::fromUtf8(
                "➔➘➙➚➛➜➝➞➟"
                "➠➡➢➣➤➥➦➧➨➩➪➫➬➭➮➯"
                "➱➲➳➴➵➶➷➸➹➺➻➼➽➾"
                "↖↗↘↙→←↑↓"
                "⇄⇅↲↳↴↵↶↷↺↻");

    mapSymbol["Mathematic"] = QString::fromUtf8(
                "＋－×÷±∵∴∈≡∝"
                "∑∏∪∩∫∮∶∧∨∷"
                "≌≈∽≠≮≯≤≥∞∠");
    mapSymbol["Table"] = QString::fromUtf8(
                "─━│┃┄┅┆┇┈┉┊┋"
                "┗┛┏┓└┘┌┐┕┙┍┑┖┚┎┒"
                "┣┫┠┨┝┥├┤┡┩┢┪┟┧┞┦┴┬┶┭┹┮┷┯┳┻"
                "┵┲┸┰┺┱┾┽┿┼"
                "╀╁╂╃╄╅╆╇╈╉╊╋╌╍╎╏"
                "═║╚╝╙╜╘╛╔╗╓╖╒╕╠╣╟╢╞╡"
                "╩╦╧╤╨╥╬╪╫"
                "╭╮╰╯╱╲╳╴╵╶╷╸╹╻╺╼╽╾╿"
                "▁▂▃▄▅▆▇█▉▊▋▌▍▎▏"
                "▐░▒▓▔▕"); // U+2500-U+257F

    mapSymbol["CJK"] = QString::fromUtf8(
                "〔〕（）〈〉《》「」"
                "『』〖〗【】［］｛｝"
                "︵︶︹︺︿﹀︽︾﹁﹂"
                "﹃﹄︻︼︷︸‘’“”");

    mapSymbol["Letter"] = QString::fromUtf8(
                "ΑΒΓΔΕΖΗΘΙΚ"
                "ΛΜΝΞΟΠΡΣΤΥ"
                "ΦΧΨΩαβγδεζ"
                "ηθικλμνξοπ"
                "ρστυφχψω");

    listSymbolName << "Table" << "Number" << "CJK" << "Letter"
                   << "Arrow" << "Mathematic";
    foreach(QString symbolName, listSymbolName) {
        addSubPane(new CharTable(mapSymbol[symbolName]), symbolName);
    }
}

CharPane::~CharPane()
{

}

void CharPane::addSubPane(QWidget *child, const QString &title)
{
    Header *header = new Header(title,this);
    m_layoutVBox->addWidget(header);
    m_layoutVBox->addWidget(child);
    connect(header, SIGNAL(toggled(bool)), child, SLOT(setVisible(bool)));
    connect(child, SIGNAL(characterSelected(QString)), SLOT(characterSelected(QString)));
}

void CharPane::characterSelected(QString ch)
{
    emit characterSelectedSignal(ch);
}

}

#include "charpane.moc"
