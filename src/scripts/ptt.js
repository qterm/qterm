QTerm.loadScript("utils.js");
QTerm.loadScript("highlight.js");
//Enable this if you have qt bindings installed.
//QTerm.loadScript("console.js");
//QTerm.loadScript("websnap.js");

QTerm.PTT = {
    Unknown : -1,
    Menu : 0,
    List : 1,
    Article : 2
}

QTerm.pageState = QTerm.PTT.Unknown;

QTerm.init = function()
{
    QTerm.osdMessage("system script loaded", QTerm.OSDType.Info, 10000);
}

QTerm.setCursorType = function(x,y)
{
    QTerm.accepted = false;
    if (QTerm.isListLineClickable(x,y)) {
        QTerm.accepted = true;
        return 7;
    }
    return -1;
}

// This is for PTT only
QTerm.setPageState = function()
{
    QTerm.accepted = true;
    var title = QTerm.getText(0);
    var bottom = QTerm.getText(QTerm.rows()-1);
    var third = QTerm.getText(2);
    QTerm.pageState = QTerm.PTT.Unknown;
    var menuList = ["【主功能表】","【電子郵件】","【聊天說話】","【個人設定】","【工具程式】"];
    var listList = ["【看板列表】","【精華文章】","【分類看板】","【休閒聊天】"];
    if (title.startsWith(menuList))
        QTerm.pageState = QTerm.PTT.Menu;
    else if (title.startsWith(listList))
        QTerm.pageState = QTerm.PTT.List;
    else if (third.indexOf("編號")!=-1)
        QTerm.pageState = QTerm.PTT.List;
    else if (bottom.indexOf("瀏覽")!=-1)
        QTerm.pageState = QTerm.PTT.Article;
    return QTerm.pageState;
}

QTerm.setSelectRect = function(x, y)
{
    var rect = [0,0,0,0];
    if (QTerm.pageState == QTerm.PTT.List && QTerm.isListLineClickable(x,y)) {
        QTerm.accepted = true;
        rect[0] = 0;
        rect[1] = y;
        rect[2] = QTerm.columns();
        rect[3] = 1;
    } else if (QTerm.pageState == QTerm.PTT.Menu) {
        QTerm.accepted = true;
        var item = QTerm.getMenuItem(x, y);
        var line = QTerm.getLine(y);
        if (item.length > 0) {
            var index = line.getText().indexOf(item);
            rect[0] = line.beginIndex(index);
            rect[1] = y
            rect[2] = line.beginIndex(index+item.length) - rect[0];
            rect[3] = 1;
        }
    }
    return rect;
}

QTerm.isListLineClickable = function(x, y)
{
    if (QTerm.pageState == QTerm.PTT.List) {
        if (x < 12 || x > QTerm.columns() - 16)
            return false;
        var text = QTerm.getText(y);
        var item = text.split(/\s+/)[1].replace(".","");
        if (item == "●")
            item = text.split(/\s+/)[2].replace(".","");
        var num = parseInt(item);
        if (item == "★" || item == num.toString()) {
            QTerm.accepted = true;
            return true;
        }
    }
    QTerm.accepted = false;
    return false
}

QTerm.getMenuItem = function(x, y)
{
    QTerm.accepted = true;
    if (QTerm.pageState != QTerm.PTT.Menu) {
        return "";
    }
    var line = QTerm.getLine(y);
    var text = line.getText();
    var pos = line.pos(x);
    var pattern = /\([a-zA-Z0-9]\)[^\s]+(\s[^\s]+)?\s+(【.*】|[^\s]+)/g;
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
    if (QTerm.pageState == QTerm.PTT.Menu) {
        var str = QTerm.getMenuItem(x,y);
        if (str == "") {
            return false;
        }
        var result = str.match(/\(([a-zA-Z0-9])\)[^\s]/);
        //var result = str.match(/[\(\[]?([a-zA-Z0-9])[\).\]]\s[^\s]+/);
        QTerm.sendString(result[1]);
        QTerm.sendParsedString("^M");
        return true;
    }
    return false;
}

QTerm.onKeyPressEvent = function(key, modifiers, text)
{
//    var msg = "The key pressed is: " + text;
//    QTerm.osdMessage(msg,1,1000);
    QTerm.accepted = false;
}

QTerm.onWheelEvent = function(delta, buttons, modifiers, pt_x, pt_y, orientation)
{
    QTerm.accepted = false;
}

QTerm.onNewData = function()
{
    QTerm.accepted = false;
    QTerm.scriptEvent("QTerm: new data");
    QTerm.highlightKeywords(/qterm|kde/ig);
    return false;
}

QTerm.antiIdle = function()
{
    QTerm.accepted = false;
}

//The script shoul get the message by itself since the code in QTerm might not be reliable for every
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

if (QTerm.qtbindingsAvailable) {
    QTerm.loadScript("console.js");
    QTerm.loadScript("websnap.js");
    QTerm.loadScript("senddelay.js");
    QTerm.loadScript("article.js");
    QTerm.onCopyArticle = function()
    {
        var text = ""
        if (QTerm.pageState != QTerm.PTT.Article)
            QTerm.osdMessage("No article to download.", QTerm.OSDType.Warning, 5000);
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

// Here is an example about how to add item to the popup menu.

QTerm.addPopupSeparator();

QTerm.onAbout = function()
{
    msg = "You are using ptt.js in QTerm " + QTerm.version() + " (C) 2009 QTerm Developers";
    QTerm.osdMessage(msg, QTerm.OSDType.Info, 10000);
}

if (QTerm.addPopupMenu( "aboutScript", "About This Script" ) ) {
        QTerm.aboutScript.triggered.connect(QTerm.onAbout);
}

QTerm.endOfArticle = function()
{
    if( QTerm.getText(QTerm.rows()-1).indexOf("100%") != -1 ) {
        return true;
    } else {
        return false;
    }

}
