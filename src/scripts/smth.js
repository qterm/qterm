var pageState = -1;
var clickableString = "";

function init()
{
    QTerm.showMessage("system script loaded", 1, 0);
}

function getCursorType(pos)
{
    return -1;
}

function getBG(color)
{
    var bg = (color & 0xf0) >> 4;
    return bg;
}

String.prototype.startsWith = function(list)
{
    for (var i = 0; i < list.length; i++) {
        var item = list[i];
        if (this.substr(0,item.length) == item)
            return true;
    }
    return false;
    //return(this.substr(0,str.length) == str);
}

String.prototype.contains = function(str) {
    return(this.indexOf(str) != -1);
}

function getText(line) {
    return QTerm.getLine(line).getText();
}

// This is for SMTH only
function setPageState()
{
    var title = getText(0);
    var bottom = getText(QTerm.rows()-1);
    pageState = -1;
    var menuList = ["主选单","聊天选单","[处理信笺选单]","工具箱选单","分类讨论区选单"];
    var listList = ["版主:","[好朋友列表]","[讨论区列表]","邮件选单","[个人定制区]"];
    var articleList = ["[阅读文章]","下面还有喔","(R)回信, (D)删除, (G)继续? [G]:"];
    if (title.startsWith(menuList))
        pageState = 0;
    if (title.startsWith(listList))
        pageState = 1;
    if (title.indexOf("水木社区 精华区公布栏")!=-1)
        pageState = 1;
    if (title.indexOf("本日十大热门话题")!=-1)
        pageState = 1;
    if (bottom.startsWith(articleList))
        pageState = 2;
    return pageState;
}

function isLineClickable(x, y)
{
    if (pageState == 1) {
        if (y >= 3 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && getText(y).search(/[^\s]/)!=-1) {
            return true;
        }
    }
    return false
}

function getClickableString(x, y)
{
    if (pageState != 0) {
        return "";
    }
    var line = QTerm.getLine(y);
    var text = line.getText();
    var pos = line.pos(x);
    var pattern = /[\(\[]?[a-zA-Z0-9][\).\]]\s?[^\s]+/g;
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

function onMouseEvent(type, button, buttons, modifiers, pt_x, pt_y)
{
    var x = QTerm.char_x(pt_x, pt_y);
    var y = QTerm.char_y(pt_x, pt_y);
    var accepted = false;
    if (type == 1 && button == 1 && modifiers == 0) {
        accepted = sendKey(x, y);
    }
    return accepted;
}

function sendKey(x, y)
{
    if (pageState == 0) {
        str = getClickableString(x,y);
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

function onKeyPressEvent(key, modifiers, text)
{
    var msg = "The key pressed is: " + text;
    QTerm.showMessage(msg,1,1000);
    return false;
}

function onWheelEvent(delta, buttons, modifiers, pt_x, pt_y, orientation)
{
    return false;
}
