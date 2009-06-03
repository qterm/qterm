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

QTerm.getBG = function(color)
{
    var bg = (color & 0xf0) >> 4;
    return bg;
}


QTerm.getText = function(line) {
    return QTerm.getLine(line).getText();
}
