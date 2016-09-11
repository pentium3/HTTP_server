
import urllib2,threading
cnt=0

class MyThread(threading.Thread):
	def __init__(self,threadname):
		threading.Thread.__init__(self,name=threadname)

	def run(self):
		print(self.getName())
		global cnt
		cnt+=1
		print("thread.no",cnt)
		while(1):
			for retry in range(10000):
				try:
					urllib2.urlopen("http://192.168.1.150/")
				except:
					print("TimeOut Error")
					pass

for i in range(0,100):
	my = MyThread('test')
	#my.setDaemon(True)
	my.start()

