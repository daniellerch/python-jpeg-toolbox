#!/usr/bin/python3

import sys
import numpy as np
from pprint import pprint
import jpeg_toolbox


r = jpeg_toolbox.load('lena.jpg')
#pprint(r)
print(r.keys())
print(r["image_width"])
#pprint(r["coef_arrays"])
#np.set_printoptions(threshold=sys.maxsize)
print(r["coef_arrays"])

