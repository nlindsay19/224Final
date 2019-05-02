import numpy as np 
from numpy import fft
import math
from scipy import signal
from skimage import filters
from skimage import io, color, transform, img_as_ubyte, img_as_float
from matplotlib import pyplot
import random
from scipy import ndimage

#mask = img_as_float(color.rgb2gray(io.imread("/Users/purvigoel/224Final/images/op.png")))
new = img_as_float(io.imread("/Users/purvigoel/Downloads/face/texture.png"))
#d = img_as_float(color.rgb2gray(io.imread("/Users/purvigoel/224Final/images/desired3.jpg")))
# new[mask < 0.9] = 0
# new[d > 0.5] = 1
# new[new > 0.55] = 1
# new[new < 0.25] = 0
for i in range(new.shape[0]):
	print(i)
	for j in range(new.shape[1]):
		for c in range(3):
			new[i,j,c] = new[i,j,c] / (1 + new[i,j,c])
io.imsave("/Users/purvigoel/Downloads/face/texture2.png", img_as_ubyte(new))

# mask = img_as_float(color.rgb2gray(io.imread("/Users/purvigoel/224Final/images/op.png")))
# new = img_as_float(color.rgb2gray(io.imread("/Users/purvigoel/224Final/images/art.jpg")))
# d = img_as_float(color.rgb2gray(io.imread("/Users/purvigoel/224Final/images/desired3.jpg")))
# new[mask < 0.9] = 0
# new[d > 0.5] = 1
# new[new > 0.55] = 1
# new[new < 0.25] = 0
# io.imsave("/Users/purvigoel/224Final/images/art.png", img_as_ubyte(new))

# caustic = color.rgb2gray(io.imread("/Users/purvigoel/224Final/images/phaseSymmetryStart_ballerina.jpg"))
# color = filters.gaussian(img_as_float(io.imread("/Users/purvigoel/224Final/images/color.jpg")), 35)

# out = np.zeros((caustic.shape[0], caustic.shape[1],3))
# for i in range(caustic.shape[0]):
# 	for j in range(caustic.shape[1]):
# 		if(caustic[i,j] > 10):
# 			out[i,j,0] = min((1.5 * caustic[i,j] * color[i,j,0])/255.0, 1.0)
# 			out[i,j,1] = min((1.5 * caustic[i,j] * color[i,j,1])/255.0, 1.0)
# 			out[i,j,2] = min((1.5 * caustic[i,j] * color[i,j,2])/255.0, 1.0)
# 		else :
# 			out[i,j,0] = caustic[i,j]/255.0
# 			out[i,j,1] = caustic[i,j]/255.0
# 			out[i,j,2] = caustic[i,j]/255.0

# io.imsave("/Users/purvigoel/224Final/images/stained.jpg", img_as_ubyte(out))
