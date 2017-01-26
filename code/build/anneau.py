import subprocess
import sys
import multiprocessing

def f(i):
	subprocess.call(["./nachos-complete", "-m", str(i), "-h", str((i+1)%int(n))])

l = []
n = sys.argv[1]
i = 0
while (i < int(n)):
	p = multiprocessing.Process(target = f, args = (i,))
	l.append(p)
	p.start()
	i = i + 1
for p in l:
	p.join()