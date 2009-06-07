QTerm.import("utils.js");
QTerm.import("highlight.js");
QTerm.import("console.js");
QTerm.import("websnap.js");

QTerm.pageState = -1;

QTerm.init = function()
{
    QTerm.showMessage("system script loaded", 1, 0);
}

QTerm.setCursorType = function(x,y,pos)
{
    QTerm.accepted = false;
    return -1;
}

// This is for SMTH only
QTerm.setPageState = function()
{
    QTerm.accepted = true;
    var title = QTerm.getText(0);
    var bottom = QTerm.getText(QTerm.rows()-1);
    var third = QTerm.getText(2);
    QTerm.pageState = -1;
    var menuList = ["主选单","聊天选单","[处理信笺选单]","工具箱选单","分类讨论区选单","系统资讯选单"];
    var listList = ["[好朋友列表]","[讨论区列表]","邮件选单","[个人定制区]"];
    var articleList = ["[十大模式]","[主题阅读]","[阅读文章]","[阅读精华区资料]","下面还有喔","(R)回信, (D)删除, (G)继续? [G]:"];
    if (title.startsWith(menuList))
        QTerm.pageState = 0;
    else if (title.startsWith(listList))
        QTerm.pageState = 1;
    else if (title.indexOf("水木社区 精华区公布栏")!=-1)
        QTerm.pageState = 1;
    else if (title.indexOf("本日十大热门话题")!=-1)
        QTerm.pageState = 1;
    else if (third.indexOf("编号")!=-1)
        QTerm.pageState = 1;
    else if (bottom.startsWith(articleList))
        QTerm.pageState = 2;
    return QTerm.pageState;
}

QTerm.isLineClickable = function(x, y)
{
    if (QTerm.pageState == 1) {
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
    if (QTerm.pageState != 0) {
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
    if (QTerm.pageState == 0) {
        str = QTerm.getClickableString(x,y);
        if (str == "") {
            return false;
        }
        result = str.match(/[\(\[]?([a-zA-Z0-9])[\).\]]\s?[^\s]+/);
        QTerm.sendString(result[1]);
        QTerm.sendString("\n");
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
