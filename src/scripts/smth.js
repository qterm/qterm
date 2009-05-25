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

function fromUtf8(utftext)
{
    var string = "";
    var i = 0;
    var c = c1 = c2 = 0;

    while ( i < utftext.length ) {

        c = utftext.charCodeAt(i);

        if (c < 128) {
            string += String.fromCharCode(c);
            i++;
        }
        else if((c > 191) && (c < 224)) {
            c2 = utftext.charCodeAt(i+1);
            string += String.fromCharCode(((c & 31) << 6) | (c2 & 63));
            i += 2;
        }
        else {
            c2 = utftext.charCodeAt(i+1);
            c3 = utftext.charCodeAt(i+2);
            string += String.fromCharCode(((c & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
            i += 3;
        }

    }

    return string;
}

String.prototype.startsWith = function(str) {
    return(this.substr(0,str.length) == str);
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
    if (title.startsWith(fromUtf8("主选单")))
        pageState = 0;
    if (title.startsWith(fromUtf8("聊天选单")))
        pageState = 0;
    if (title.startsWith(fromUtf8("[处理信笺选单]")))
        pageState = 0;
    if (title.startsWith(fromUtf8("工具箱选单")))
        pageState = 0;
    if (title.startsWith(fromUtf8("版主:")))
        pageState = 1;
    if (title.startsWith(fromUtf8("[好朋友列表]")))
        pageState = 1;
    if (title.startsWith(fromUtf8("[讨论区列表]")))
        pageState = 1;
    if (title.startsWith(fromUtf8("邮件选单")))
        pageState = 1;
    if (title.startsWith(fromUtf8("[个人定制区]")))
        pageState = 1;
    if (title.contains(fromUtf8("水木社区 精华区公布栏")))
        pageState = 1;
    if (title.contains(fromUtf8("本日十大热门话题")))
        pageState = 1;
    if (bottom.startsWith(fromUtf8("[阅读文章]")))
        pageState = 2;
    if (bottom.startsWith(fromUtf8("下面还有喔")))
        pageState = 2;
    if (bottom.startsWith(fromUtf8("(R)回信, (D)删除, (G)继续? [G]:")))
        pageState = 2;
    return pageState;
}

function isLineClickable(x, y)
{
    if (pageState == 1) {
        if (y >= 3 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16) {
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
    var text = getText(y);
    var pos = QTerm.pos(x,y);
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
