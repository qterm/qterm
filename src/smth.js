function init()
{
    QTerm.showMessage("system script loaded");
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

function startsWith(str, target)
{
    return (str.substr(0,target.length) == target)
}

// This is for SMTH only
function setPageState()
{
    var title = QTerm.getText(0);
    var bottom = QTerm.getText(QTerm.rows()-1);
    if (startsWith(title, fromUtf8("主选单")))
        return 0;
    if (startsWith(title, fromUtf8("聊天选单")))
        return 0;
    if (startsWith(title, fromUtf8("[处理信笺选单]")))
        return 0;
    if (startsWith(title, fromUtf8("版主:")))
        return 1;
    if (startsWith(title, fromUtf8("[好朋友列表]")))
        return 1;
    if (startsWith(title, fromUtf8("[讨论区列表]")))
        return 1;
    if (startsWith(title, fromUtf8("邮件选单")))
        return 1;
    if (startsWith(title, fromUtf8("[个人定制区]")))
        return 1;
    if (title.indexOf(fromUtf8("水木社区 精华区公布栏")) > 0)
        return 1;
    if (title.indexOf(fromUtf8("本日十大热门话题")) > 0)
        return 1;
    if (startsWith(bottom, fromUtf8("[阅读文章]")))
        return 2;
    if (startsWith(bottom, fromUtf8("下面还有喔")))
        return 2;
    if (startsWith(bottom, fromUtf8("(R)回信, (D)删除, (G)继续? [G]:")))
        return 2;
    return -1;
}
