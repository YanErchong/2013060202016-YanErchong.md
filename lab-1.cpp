import threading
import time
import os

num = 0
mutex = threading.Lock()
full = threading.Event()
zero = threading.Event()

zero.set()
full.clear()

class ConsumerThread(threading.Thread):
	def run(self):
		global num
		global zero
		global full

		while True:
			if mutex.acquire(1):
				if zero.isSet():
					mutex.release()
					continue

			num = num - 1
			msg = "Consumer:%d -> %d" % (num+1, num)
			print msg

			full.clear()
			if num == 0:
				zero.set()

			mutex.release()

class ProducerThread(threading.Thread):
	def run(self):
		global num
		global zero
		global full

		while True:
			if mutex.acquire(1):
				if full.isSet():
					mutex.release()
					continue

			num = num + 1
			msg = "Producer:%d -> %d" % (num-1, num)
			print msg

			zero.clear()
			if num == 10:
				full.set()
			
			mutex.release()

class StopThread(threading.Thread):
	def __init__(self, secs):
		threading.Thread.__init__(self)
		self.secs = secs

	def run(self):
		while self.secs:
			self.secs -= 1
			time.sleep(1)

		os._exit(0)

if __name__ == '__main__':
	cs = ConsumerThread()
	pt = ProducerThread()
	st = StopThread(1)
	cs.start()
	pt.start()
	st.start()
	input()
