# 连续下载 4 片文章
import qterm
import sys, time

f=open("/home/kingson/test.txt","w")

lp=long(sys.argv[0])

for i in range(4):
	qterm.sendString(lp,"r")
	f.write(qterm.copyArticle(lp))
	f.write("="*80)
	f.write("\n\n")
	qterm.sendString(lp,"q")
	qterm.sendString(lp,"j")
	time.sleep(1)
f.close()
