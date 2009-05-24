function init()
{
    QTerm.showMessage("system script loaded", 1, 0);
}

function getCursorType(pos)
{
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

// This is for SMTH only
function setPageState()
{
    var title = QTerm.getText(0);
    var bottom = QTerm.getText(QTerm.rows()-1);
    if (title.startsWith(fromUtf8("主选单")))
        pageState = 0;
    if (title.startsWith(fromUtf8("聊天选单")))
        pageState = 0;
    if (title.startsWith(fromUtf8("[处理信笺选单]")))
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

function getClickableString(x, y, pos)
{
    if (pageState != 0) {
        return "";
    }
    var text = QTerm.getText(y);
    var pattern = /[\(\[]?[a-zA-Z0-9][\).\]]\s[^\s]+/g;
    var result;
    while ((result = pattern.exec(text)) != null) {
        var item = result[0];
        if (pos > result.index && pos < result.index + item.length) {
            return item;
        }
        if (pattern.lastIndex == 0)
            return text;
    }
    return ""
}
