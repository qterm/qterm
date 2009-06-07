String.prototype.startsWith = function(list)
{
    for (var i = 0; i < list.length; i++) {
        var item = list[i];
        if (this.substr(0,item.length) == item)
            return true;
    }
    return false;
}

String.prototype.contains = function(str) {
    return(this.indexOf(str) != -1);
}

String.prototype.trim = function() {
        return this.replace(/^\s+|\s+$/g,"");
}

String.prototype.ltrim = function() {
        return this.replace(/^\s+/,"");
}

String.prototype.rtrim = function() {
        return this.replace(/\s+$/,"");
}

QTerm.Attr = {
    Bold : 0x01,
    Dimm : 0x02,
    Default : 0x04,
    Underline: 0x08,
    Blink : 0x10,
    Reverse : 0x40,
    Invisible : 0x80
}

QTerm.Color = {
    Black : 0,
    Red : 1,
    Green : 2,
    Yellow : 3,
    Blue : 4,
    Magenta : 5,
    Cyan : 6,
    White : 7,
    Highlight : 8
}

QTerm.setFG = function(color)
{
    return color & 0x0f;
}

QTerm.getFG = function(color)
{
    return color & 0x0f;
}

QTerm.setBG = function(color)
{
    return (color << 4) & 0x0f;
}

QTerm.getBG = function(color)
{
    return (color & 0xf0) >> 4;
}

QTerm.setColor = function(color)
{
    return color & 0x00ff;
}

QTerm.getColor = function(color)
{
    return color & 0x00ff;
}

QTerm.setAttr = function(attr)
{
    return (attr << 8) & 0xff00;
}

QTerm.getAttr = function(attr)
{
    return (attr & 0xff00) >> 8;
}

QTerm.defaultColor = function()
{
    return setFG(QTerm.Color.White)|setBG(QTerm.Color.Black);
}

QTerm.OSDType = {
    None : 0,
    Info : 1,
    Warning : 2,
    Error : 3
}

QTerm.getText = function(line) {
    return QTerm.getLine(line).getText();
}
