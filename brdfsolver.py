#!/usr/bin/env python
import sys

import numpy as np 
from numpy import fft
import math
from scipy import optimize
from skimage import io, color, transform, img_as_ubyte, img_as_float
import random
import time

start = time.time()
print("starting")
### REDS ####
f = open("reds.txt", "r")
Alist = []
for num in f:
	Alist.append(float(num))
f.close()

f = open("desired_reds.txt", "r")
Blist = []
n = 0
for num in f:
	Blist.append(float(num))
	n += 1
f.close()


A = np.array(Alist)
A = np.reshape(A, (n, 400))

B = np.array(Blist)
B = np.reshape(B, (n))

def residuals(x):
	return B - np.matmul(A, x)

x0 = np.ones(400)
res = optimize.least_squares(residuals, x0, bounds=(0, 255), max_nfev = 100, verbose=2)
print(res)
f = open("solved_reds.txt", "w")
for i in res.x:
	num = i
	if(num < 0.001):
		num = 0
	f.write(str(num))
	f.write("\n")
f.close()

### GREENS ###
f = open("greens.txt", "r")
Alist = []
for num in f:
	Alist.append(float(num))
f.close()

f = open("desired_greens.txt", "r")
Blist = []
n = 0
for num in f:
	Blist.append(float(num))
	n += 1
f.close()


A = np.array(Alist)
A = np.reshape(A, (n, 400))

B = np.array(Blist)
B = np.reshape(B, (n))

x0 = np.ones(400)
res = optimize.least_squares(residuals, x0,bounds=(0, 255), max_nfev = 100, verbose=2)
f = open("solved_greens.txt", "w")
for i in res.x:
	num = i
	if(num < 0.001):
		num = 0
	f.write(str(num))
	f.write("\n")
f.close()

##### BLUES #####

f = open("blues.txt", "r")
Alist = []
for num in f:
	Alist.append(float(num))
f.close()

f = open("desired_blues.txt", "r")
Blist = []
n = 0
for num in f:
	Blist.append(float(num))
	n += 1
f.close()


A = np.array(Alist)
A = np.reshape(A, (n, 400))

B = np.array(Blist)
B = np.reshape(B, (n))

x0 = np.ones(400)
res = optimize.least_squares(residuals, x0,bounds=(0, 255), max_nfev = 100, verbose=2)
print(res)
f = open("solved_blues.txt", "w")
for i in res.x:
	num = i
	if(num < 0.001):
		num = 0
	f.write(str(num))
	f.write("\n")
f.close()

end = time.time()

print(end-start)