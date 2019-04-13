import numpy as np 
from numpy import fft
import math
from scipy import signal
from skimage import io, color, transform, img_as_ubyte
from matplotlib import pyplot

#image = (io.imread("/Users/purvigoel/Downloads/depthMap.png"))
image = color.rgb2gray(image)

imageFFT = fft.fft2(image)

scales = 2
orientations = 20

sigmaF = 0.85
wavelengthIncrement = 1.3
thetaStd = np.pi / orientations / 1.2

phaseSymmetry = np.zeros((image.shape[0], image.shape[1]))

symmetryTotal = np.zeros((image.shape[0], image.shape[1]))
amplitudeTotal = np.zeros((image.shape[0], image.shape[1]))
noiseAtOrientation = np.zeros((image.shape[0], image.shape[1]))
floor =  np.zeros((image.shape[0], image.shape[1]))
for a in range(1,orientations + 1):
	centerAngle = (a - 1) * np.pi/orientations

	print(a)
	wavelength = 1

	symmetryAtOrientation = np.zeros((image.shape[0], image.shape[1]))
	amplitudeAtOrientation = np.zeros((image.shape[0], image.shape[1]))
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

				elementRadial = np.exp(-1 * np.power( np.log(normalizedRadius/ (centerFrequency / 0.5)),2) / (2 * np.log(sigmaF) * np.log(sigmaF)) )
				theta = math.atan2(-y,x)
				deltaSin = math.sin(theta) * math.cos(centerAngle) - math.cos(theta) * math.sin(centerAngle)
				deltaCosine = math.cos(theta) * math.cos(centerAngle) + math.sin(theta) * math.sin(centerAngle)

				deltaTheta = abs(math.atan2(deltaSin, deltaCosine))

				elementAngular = np.exp((-1 * deltaTheta * deltaTheta) / (2 * thetaStd * thetaStd))

				kernel[i, j] = elementAngular * elementRadial
				if(kernel[i,j] < 0.001):
					kernel[i,j] = 0
		wavelength = wavelength * wavelengthIncrement
	
		kernel = fft.fftshift(kernel)
		convolved = imageFFT * kernel

		s1 = np.array(kernel.shape)
		s2 = np.array(imageFFT.shape)

		size = s1 + s2 - 1


		fsize = 2 ** np.ceil(np.log2(size)).astype(int)
		fslice = tuple([slice(0, int(sz)) for sz in size])

		convolved = fft.ifft2(convolved)[fslice]
		
		evens = np.real(convolved)
		odds = np.imag(convolved)
		muR = sigmaF * math.sqrt(np.pi/2)
		sigma2R = (4 - np.pi) * sigmaF * sigmaF/ 2
		sigmaR = math.sqrt(sigma2R)
		T = muR + scales * sigmaR
		amplitude = np.sqrt(np.power(evens, 2) + np.power(odds, 2))
		#print(amplitude)

		amplitudeAtOrientation += amplitude
		symmetryAtOrientation += np.maximum((np.abs(evens) - np.abs(odds)), floor)
	
	amplitudeTotal += np.add(amplitudeAtOrientation, 0.00001)
	symmetryTotal += symmetryAtOrientation

phaseSymmetry = np.divide(symmetryTotal , amplitudeTotal)
#io.imsave("/Users/purvigoel/Desktop/phaseSymmetry.jpg", img_as_ubyte(phaseSymmetry))
#pyplot.imshow(np.flipud(phaseSymmetry))
#pyplot.show()
