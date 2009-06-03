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
    return 0x7;
}

QTerm.defaultAttr = function()
{
    return 0x4;
}

QTerm.getText = function(line) {
    return QTerm.getLine(line).getText();
}
