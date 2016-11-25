QTerm.loadScript("utils.js");
QTerm.loadScript("highlight.js");
QTerm.loadScript("google.js");

QTerm.SMTH= {
    Unknown : -1,
    Menu : 0,
    List : 1,
    Article : 2,
    Top10 : 3,
    Edit: 4
}

QTerm.pageState = QTerm.SMTH.Unknown;

QTerm.init = function()
{
    QTerm.osdMessage(qsTr("System script loaded"), QTerm.OSDType.Info, 10000);
    if (QTerm.addPopupMenu( "aboutScript", qsTr("About This Script") ) ) {
        QTerm.aboutScript.triggered.connect(QTerm.onAbout);
    }

}

QTerm.setCursorType = function(x,y)
{
    QTerm.accepted = false;
    if (QTerm.pageState == QTerm.SMTH.Top10) {
        QTerm.accepted = true;
        if (x < 12) {
            return 6;
        } else if (y >= 2 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && QTerm.getText(y).search(/[^\s]/)!=-1) {
            return 7;
        } else {
            return 9;
        }
    }
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
    else if (title.indexOf("本日十大热门话题")!=-1 && bottom.indexOf("查阅帮助信息")!=-1)
        QTerm.pageState = QTerm.SMTH.Top10;
    else if (third.indexOf("编号")!=-1)
        QTerm.pageState = QTerm.SMTH.List;
    else if (bottom.startsWith(articleList))
        QTerm.pageState = QTerm.SMTH.Article;
    else if (bottom.indexOf("Ctrl-Q")!=-1)
        QTerm.pageState = QTerm.SMTH.Edit;
    return QTerm.pageState;
}

QTerm.setSelectRect = function(x, y)
{
    var rect = [0,0,0,0];
    if (QTerm.pageState == QTerm.SMTH.List && QTerm.isListLineClickable(x,y)) {
        QTerm.accepted = true;
        rect[0] = 0;
        rect[1] = y;
        rect[2] = QTerm.columns();
        rect[3] = 1;
    } else if (QTerm.pageState == QTerm.SMTH.Menu) {
        QTerm.accepted = true;
        var item = QTerm.getMenuItem(x, y);
        var line = QTerm.getLine(y);
        if (item.length > 0) {
            var index = line.getText().indexOf(item);
            rect[0] = line.beginIndex(index);
            rect[1] = y;
            rect[2] = line.beginIndex(index+item.length) - rect[0];
            rect[3] = 1;
        }
    } else if (QTerm.pageState == QTerm.SMTH.Top10) {
        if (y >= 2 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && QTerm.getText(y).search(/[^\s]/)!=-1) {
            QTerm.accepted = true;
            rect[0] = 0;
            rect[1] = y;
            rect[2] = QTerm.columns();
            rect[3] = 1;
        }
    }
    return rect;
}

QTerm.isListLineClickable = function(x, y)
{
    // Copied from the source code of QTerm
    if (QTerm.pageState == QTerm.SMTH.List) {
        if (y >= 3 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && QTerm.getText(y).search(/[^\s]/)!=-1) {
            QTerm.accepted = true;
            return true;
        }
    } else if (QTerm.pageState == QTerm.SMTH.Top10) {
        if (y >= 2 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && y % 2 == 1 && QTerm.getText(y).search(/[^\s]/)!=-1) {
            QTerm.accepted = true;
            return true;
        }
    }
    QTerm.accepted = false;
    return false
}

QTerm.getMenuItem= function(x, y)
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
    if (QTerm.getUrl().length > 0)
        return false;

    var result;
    if (QTerm.pageState == QTerm.SMTH.Article && x < 12) {
            if( QTerm.getText(QTerm.rows()-1).indexOf("%") != -1 ) {
                QTerm.sendParsedString("^[[D");
                QTerm.sendParsedString("^[[D");
            } else
                QTerm.sendParsedString("^[[D");
            return true;
    } else if (QTerm.pageState == QTerm.SMTH.Menu) {
        str = QTerm.getMenuItem(x,y);
        if (str == "") {
            return false;
        }
        result = str.match(/[\(\[]?([a-zA-Z0-9])[\).\]]\s?[^\s]+/);
        QTerm.sendString(result[1]);
        QTerm.sendParsedString("^M");
        return true;
    } else if (QTerm.pageState == QTerm.SMTH.Top10 && (y >= 2 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && QTerm.getText(y).search(/[^\s]/)!=-1)) {
        var text = QTerm.getText(y - y%2);
        result = QTerm.getText(y - y%2).match(/\s+(\d+)\s+/);
        QTerm.sendString(result[1]);
        if (y%2 == 0) {
            QTerm.sendParsedString("s");
        } else {
            QTerm.sendParsedString("^M");
        }
    }
    return false;
}

QTerm.onKeyPressEvent = function(key, modifiers, text)
{
//    var msg = "The key pressed is: " + text;
//    QTerm.osdMessage(msg,1,1000);
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
    QTerm.scriptEvent("QTerm: new data");
    QTerm.highlightKeywords(/qterm|kde/ig);
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

if (QTerm.qtbindingsAvailable) {
    QTerm.loadScript("console.js");
    QTerm.loadScript("senddelay.js");
    QTerm.loadScript("article.js");
    QTerm.onCopyArticle = function()
    {
        var text = ""
        if (QTerm.pageState != QTerm.SMTH.Article)
            QTerm.osdMessage(qsTr("No article to download"), QTerm.OSDType.Warning, 5000);
        else
            text = QTerm.Article.getArticle();
        QTerm.accepted = true;
        return text;
    }
} else {
    QTerm.onCopyArticle = function()
    {
        QTerm.accepted = false;
        return "";
    }

}

QTerm.addPopupSeparator();

QTerm.onAbout = function()
{
    msg = qsTr("You are using smth.js in QTerm %1 (C) 2009-2010 QTerm Developers").arg(QTerm.version());;
    QTerm.osdMessage(msg, QTerm.OSDType.Info, 10000);
}

QTerm.endOfArticle = function()
{
    if( QTerm.getText(QTerm.rows()-1).indexOf("%") == -1 ) {
        return true;
    } else {
        return false;
    }

}
