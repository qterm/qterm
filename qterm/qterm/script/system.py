import qterm, string

def autoReply(lp):
	# not message
	if(qterm.caretY(lp)!=2):
		return
	text = qterm.getText(lp,0)
	# sender
	lst = text.split()
	sender = lst[0] 

	# who am i, this works when user ID in status bar
	# so better set yourself
	# text = qterm.getText(lp,qterm.rows(lp)-1)
	#lst = string.split(text,"]")
	#text = lst[2]
	#lst = string.split(text,"[")
	#me = lst[1]
	me = "kingson"
	msg = qterm.getText(lp,1)
	reply="i am "+me+"'s autoreply script, can i help you?"

	if(msg.find("ft")!=-1):
		reply = sender+" ,why ft?"
	elif(msg.find("怎么")!=-1 or msg.find("为什么")!=-1):
		reply = "我也不知道耶"

	qterm.sendString(lp,"r")
	qterm.sendString(lp,reply)
	qterm.sendParsedString(lp,"^M")
	
def antiIdle(lp):
	qterm.sendParsedString(lp,"^@")
	print "antiIdle"

