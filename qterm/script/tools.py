# various tools

# Left Arrow <--
def sendLeft(qterm,lp):
	qterm.sendParsedString(lp,"^[[C")
# Left Arrow -->
def sendRight(qterm,lp):
	qterm.sendParsedString(lp,"^[[D")
# Up Arrow  
def sendUp(qterm,lp):
	qterm.sendParsedString(lp,"^[[A")
# Down Arrow  
def sendDown(qterm,lp):
	qterm.sendParsedString(lp,"^[[B")
# Page Up
def sendPageUp(qterm,lp):
	qterm.sendParsedString(lp,"^[[5")
# Page Down
def sendPageDown(qterm,lp):
	qterm.sendParsedString(lp,"^[[6")
# Enter 
def sendDown(qterm,lp):
	qterm.sendParsedString(lp,"^M")

# Page State
# Firebird BBS only
def getPageState(qterm,lp):
	pageState="unknown"
	lastline=qterm.getText(lp,qterm.rows(lp)-1)
	if(lastline.find("阅读文章")!=-1 or
		lastline.find("阅读精华区")!=-1 or
		lastline.find("下面还有")!=-1 or
		lastline.find("回信")!=-1):
		pageState="reading"
	elif(lastline.find("时间")!=-1 and
		lastline.find("使用者")!=-1):
		pageState="list"

	return pageState

# analyze article list	
# return a dictionary
# {"num":xxx,"sign":"","id":"","date":"","title":""}
def getListInfo(text):
	lst=text.split()
	info["num"]=long(lst[0])
	info["sign"]=lst[1]
	info["id"]=lst[2]
	info["date"]=lst[3]+lst[4]
	info["title"]=lst[5]
	return info

# get the current user ID
# only for FireBird BBS
def getCurrentID(qterm,lp):
	text = qterm.getText(lp,qterm.rows(lp)-1)
	if(text.find("使用者")==-1)	# wrong status
		return
	lst = string.split(text,"]")
	text = lst[2]
	lst = string.split(text,"[")
	return lst[1]


