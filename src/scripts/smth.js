QTerm.import("utils.js");
QTerm.import("highlight.js");
//Enable this if you have qt bindings installed.
//QTerm.import("console.js");
//QTerm.import("websnap.js");

QTerm.SMTH= {
    Unknown : -1,
    Menu : 0,
    List : 1,
    Article : 2
}

QTerm.pageState = QTerm.SMTH.Unknown;

QTerm.init = function()
{
    QTerm.showMessage("system script loaded", QTerm.OSDType.Info, 10000);
}

QTerm.setCursorType = function(x,y)
{
    QTerm.accepted = false;
    return -1;
}

// VERY naive/straight forward code, for SMTH only
QTerm.setPageState = function()
{
    QTerm.accepted = true;
    var title = QTerm.getText(0);
    var bottom = QTerm.getText(QTerm.rows()-1);
    var third = QTerm.getText(2);
    QTerm.pageState = QTerm.SMTH.Unknown;
    var menuList = ["主选单","聊天选单","[处理信笺选单]","工具箱选单","分类讨论区选单","系统资讯选单"];
    var listList = ["[好朋友列表]","[讨论区列表]","邮件选单","[个人定制区]"];
    var articleList = ["[十大模式]","[主题阅读]","[阅读文章]","[阅读精华区资料]","下面还有喔","(R)回信, (D)删除, (G)继续? [G]:"];
    // The functions used here is defined in utils.js
    if (title.startsWith(menuList))
        QTerm.pageState = QTerm.SMTH.Menu;
    else if (title.startsWith(listList))
        QTerm.pageState = QTerm.SMTH.List;
    else if (title.indexOf("水木社区 精华区公布栏")!=-1)
        QTerm.pageState = QTerm.SMTH.List;
    else if (third.indexOf("编号")!=-1)
        QTerm.pageState = QTerm.SMTH.List;
    else if (bottom.startsWith(articleList))
        QTerm.pageState = QTerm.SMTH.Article;
    return QTerm.pageState;
}

QTerm.isLineClickable = function(x, y)
{
    // Copied from the source code of QTerm
    if (QTerm.pageState == QTerm.SMTH.List) {
        if (y >= 3 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && QTerm.getText(y).search(/[^\s]/)!=-1) {
            QTerm.accepted = true;
            return true;
        }
    }
    QTerm.accepted = false;
    return false
}

QTerm.getClickableString = function(x, y)
{
    QTerm.accepted = true;
    if (QTerm.pageState != QTerm.SMTH.Menu) {
        return "";
    }
    var line = QTerm.getLine(y);
    var text = line.getText();
    var pos = line.pos(x);
    var pattern = /\b[\(\[]?[a-zA-Z0-9][\)\]]\s?[^\s]+/g;
    var result;
    var clickableString = "";
    while ((result = pattern.exec(text)) != null) {
        clickableString = result[0];
        if (pos > result.index && pos < result.index + clickableString.length) {
            return clickableString;
        }
        if (pattern.lastIndex == 0)
            return "";
    }
    return ""
}

QTerm.onMouseEvent = function(type, button, buttons, modifiers, pt_x, pt_y)
{
    var x = QTerm.charX(pt_x, pt_y);
    var y = QTerm.charY(pt_x, pt_y);
    var accepted = false;
    if (type == 1 && button == 1 && modifiers == 0) {
        accepted = QTerm.sendKey(x, y);
    }
    QTerm.accepted = accepted;
}

QTerm.sendKey = function(x, y)
{
    // Only handle the menu case
    if (QTerm.pageState == QTerm.SMTH.Menu) {
        str = QTerm.getClickableString(x,y);
        if (str == "") {
            return false;
        }
        result = str.match(/[\(\[]?([a-zA-Z0-9])[\).\]]\s?[^\s]+/);
        QTerm.sendString(result[1]);
        QTerm.sendParsedString("^M");
        return true;
    }
    return false;
}

QTerm.onKeyPressEvent = function(key, modifiers, text)
{
//    var msg = "The key pressed is: " + text;
//    QTerm.showMessage(msg,1,1000);
    QTerm.accepted = false;
}

QTerm.onWheelEvent = function(delta, buttons, modifiers, orientation, pt_x, pt_y)
{
    QTerm.accepted = false;
}

QTerm.onNewData = function()
{
    QTerm.accepted = false;
    // This will highlight qterm and kde, function defined in highlight.js
    QTerm.highlightKeywords(/qterm|kde/ig);
// This is a ugly way to download article
//    if (QTerm.Article.downloading)
//        QTerm.Article.downloadArticle();
    return false;
}

QTerm.antiIdle = function()
{
    QTerm.accepted = false;
}

//The script should get the message by itself since the code in QTerm might not be reliable for every
//site.
QTerm.autoReply = function()
{
    QTerm.accepted = false;
}

QTerm.checkUrl = function(x, y)
{
    QTerm.accepted = false;
    return "";
}

QTerm.checkIP = function(x, y)
{
    QTerm.accepted = false;
    return "";
}

QTerm.onTelnetState = function(state)
{
    QTerm.accepted = false;
    return;
}

QTerm.onZmodemState = function(type, value, state)
{
    QTerm.accepted = false;
    return;
}

// Here is an example about how to add item to the popup menu.

QTerm.addPopupSeparator();

QTerm.onAbout = function()
{
    msg = "You are using smth.js in QTerm " + QTerm.version() + " (C) 2009 QTerm Developers";
    QTerm.showMessage(msg, QTerm.OSDType.Info, 10000);
}

if (QTerm.addPopupMenu( "aboutScript", "About This Script" ) ) {
        QTerm.aboutScript.triggered.connect(QTerm.onAbout);
}

/*
QTerm.import("article.js");

QTerm.onArticle = function()
{
    QTerm.Article.getArticle();
}

if (QTerm.addPopupMenu( "article", "Download Article" ) ) {
        QTerm.article.triggered.connect(QTerm.onArticle);
}
*/
