import numpy as np 
from numpy import fft
import math
from scipy import signal
from skimage import filters
from skimage import io, color, transform, img_as_ubyte, img_as_float
from matplotlib import pyplot
import random
from scipy import ndimage


#https://www.peterkovesi.com/papers/ai97.pdf
#https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=1527851
mask = color.rgb2gray(io.imread("/Users/purvigoel/224Final/images/bunny_mask.png"))
image = (io.imread("/Users/purvigoel/224Final/images/bunny.png"))
image = color.rgb2gray(image)
image[mask < 0.2] = -1000
image[image < 0.01] = -1000
imageFFT = fft.fft2(image)

scales = 2 #4
orientations = 4 #6 

sigmaF = 0.55 #0.65
wavelengthIncrement = 3 #1.7
thetaStd = np.pi / orientations / 1.2

phaseSymmetry = np.zeros((image.shape[0], image.shape[1]))

symmetryTotal = np.zeros((image.shape[0], image.shape[1]))
amplitudeTotal = np.zeros((image.shape[0], image.shape[1]))
noiseAtOrientation = np.zeros((image.shape[0], image.shape[1]))
floor =  np.zeros((image.shape[0], image.shape[1]))

bank = []
for a in range(1,orientations + 1):
	centerAngle = (a - 1) * np.pi/orientations

	print(a)
	wavelength = 7 #3

	symmetryAtOrientation = np.zeros((image.shape[0], image.shape[1]))
	amplitudeAtOrientation = np.zeros((image.shape[0], image.shape[1]))
	kernels = []
	for n in range(scales):
		kernel = np.zeros((image.shape[0], image.shape[1]))
		centerFrequency = 1/wavelength
		for i in range(kernel.shape[0]):
			for j in range(kernel.shape[1]):
				y = i - (kernel.shape[0]/2)
				x = j-(kernel.shape[1]/2)

				normalizedY = y / (kernel.shape[0]/2)
				normalizedX = x / (kernel.shape[1]/2)

				normalizedRadius = math.sqrt(normalizedY * normalizedY + normalizedX * normalizedX)

				elementRadial = np.exp(-1 * np.power( (normalizedRadius/ (centerFrequency / 0.5)),2) / (2 * (sigmaF) * (sigmaF)) )
				theta = math.atan2(-y,x)
				deltaSin = math.sin(theta) * math.cos(centerAngle) - math.cos(theta) * math.sin(centerAngle)
				deltaCosine = math.cos(theta) * math.cos(centerAngle) + math.sin(theta) * math.sin(centerAngle)

				deltaTheta = abs(math.atan2(deltaSin, deltaCosine))

				elementAngular = np.exp((-1 * deltaTheta * deltaTheta) / (2 * thetaStd * thetaStd))

				kernel[i, j] = elementAngular * elementRadial
				if(kernel[i,j] < 0.001):
					kernel[i,j] = 0
		kernel = fft.fftshift(kernel)
		kernels.append(kernel)
		wavelength = wavelength * wavelengthIncrement
	
	bank.append(kernels)


def calculatePhaseSymmetry(imageFFT, phaseSymmetry, symmetryTotal, amplitudeTotal):
	for a in range(1,orientations + 1):
		symmetryAtOrientation = np.zeros((image.shape[0], image.shape[1]))
		amplitudeAtOrientation = np.zeros((image.shape[0], image.shape[1]))
		for n in range(scales):		
			kernel = bank[a - 1][n]		
			convolved = imageFFT * kernel
			s1 = np.array(kernel.shape)
			s2 = np.array(imageFFT.shape)
			convolved = fft.ifft2(convolved)
			
			evens = np.real(convolved)
			odds = np.imag(convolved)
			amplitude = np.sqrt(np.power(evens, 2) + np.power(odds, 2)) 
			
			amplitudeAtOrientation += amplitude
			symmetryAtOrientation += np.maximum((np.abs(evens) - np.abs(odds))  , floor)
		
		amplitudeTotal += np.add(amplitudeAtOrientation, 0.00001)
		symmetryTotal += symmetryAtOrientation

	phaseSymmetry = np.divide(symmetryTotal , amplitudeTotal)
	phaseSymmetry[mask < 0.2] = 0.0
	return phaseSymmetry

def proposeImageVector(mask, xmin, xmax, ymin, ymax, length):
	vector = []
	for i in range(vectorLength):
		squareLength = random.randint(1, 5) #5
		x = min(random.randint(xmin, xmax-math.ceil(squareLength/2) - 1), xmax)
		y = min(random.randint(ymin, ymax-math.ceil(squareLength/2) - 1), ymax)

		while(mask[y + math.floor(squareLength/2), x + math.floor(squareLength/2)] < 0.1):
			if(mask[y,x] > 0.1):
				break
			squareLength = random.randint(1, 5) #5
			x = random.randint(xmin, xmax-math.ceil(squareLength/2) - 1)
			y = random.randint(ymin, ymax-math.ceil(squareLength/2) - 1)

		vector.append(x)
		vector.append(y)
		vector.append(squareLength)
	return vector



def vectorToImage(image, vector, desired):
	new = image.copy()
	circles = np.zeros((image.shape[0], image.shape[1]))
	for i in range(0, len(vector), 3):
		x = vector[i]
		y = vector[i+1]
		squareLength = vector[i+2] * 2

		for ly in range(squareLength):
			for lx in range(squareLength):
				if(y + ly < image.shape[0] and x + lx < image.shape[1]):
					radius = squareLength/2
					candidateY = ly - radius
					candidateX = lx - radius 
					if(candidateY * candidateY + candidateX * candidateX < (radius * radius)):
						circles[ly + y][lx + x] = 1

	blurred = filters.gaussian(circles)
	new[blurred > 0.1] = desired[blurred>0.1]

	return new

def shiftOldVector(bestVector, xmin, xmax, ymin, ymax):
	vector = []

	for i in range(0, len(bestVector), 3):
		x = bestVector[i]
		y = bestVector[i+1]
		squareLength = bestVector[i+2]
		r = random.randint(1, 5)
		if(r < 2): #6
			change = random.randint(-1, 1)
			x += change
			if x < xmin:
				x += xmin
			elif x > xmax:
				x -= xmax
		r = random.randint(1, 5)
		if(r < 2):#6
			change = random.randint(-1,1) 
			y += change
			if y < ymin:
				y += ymin
			elif y > ymax:
				y -= ymax

		r = random.randint(1, 5)
		if(r < 2):#6
			change = random.randint(-2,2)
			squareLength += change
			if squareLength <= 1:
				squareLength += abs(change)
			if squareLength > 5:
				squareLength -= abs(change)
		vector.append(x)
		vector.append(y)
		vector.append(squareLength)

	return vector

def maskMinMax(mask):
	minX = 10000
	minY = 100000
	maxX = 0
	maxY = 0
	for i in range(mask.shape[0]):
		for j in range(mask.shape[1]):
			if(mask[i,j] > 0.1):
				if i < minY:
					minY = i
				elif i > maxY:
					maxY = i

				if j < minX:
					minX = j 
				elif j > maxX:
					maxX = j 
	return minX, maxX, minY, maxY

def printImage(image, filename):
	saveImage = image.copy()
	saveImage[image < 0]= 0
	saveImage[image >1] = 1
	io.imsave(filename, img_as_ubyte(saveImage))

phaseSymmetry = calculatePhaseSymmetry(imageFFT, phaseSymmetry, symmetryTotal, amplitudeTotal)
io.imsave("/Users/purvigoel/224Final/images/phaseSymmetry.jpg", img_as_ubyte(phaseSymmetry))

image = io.imread("/Users/purvigoel/224Final/images/bunny_desired.png") 
image = img_as_float(color.rgb2gray(image))

desired = phaseSymmetry

bestDiff = 10000000
bestImage = np.zeros((image.shape[0], image.shape[1]))

success = 0
attempt = 0

bestVector = []
printcounter = 0
xmin, xmax, ymin, ymax = maskMinMax(mask)
eps = 1

while(success < 2000 and attempt < 100):
	r = random.randint(1, 10)
	attempt += 1
	vector = []
	if(r < 5 or len(bestVector) == 0):
		vectorLength = random.randint(50, 100)
		vector = proposeImageVector(mask, xmin, xmax, ymin, ymax, vectorLength)
		proposal = vectorToImage(image, vector, desired)
	else:

		vector = shiftOldVector(bestVector, xmin, xmax, ymin, ymax)
		proposal = vectorToImage(image, vector, desired)

	diff = np.sum(np.abs(proposal - desired)) + len(vector)
	
	if attempt > 50:
		print("attempt", attempt)
	if attempt == 100 or attempt == 150 or attempt == 198:
		eps *= 1.25#2

	if(diff < bestDiff) or abs(bestDiff - diff) < eps:
		eps = 0.5
		success += 1
		print( diff, success)
		attempt = 0

		bestDiff = diff
		bestImage = proposal.copy()
		bestVector = vector	

	printcounter += 1
	if(printcounter == 500):
		printcounter = 0
		printImage(bestImage, "/Users/purvigoel/224Final/images/otmCaustic.jpg")

printImage(bestImage, "/Users/purvigoel/224Final/images/otmCaustic.jpg")
