#!/usr/bin/python3

import sys
import numpy as np
from pprint import pprint
import jpeg_toolbox

r = jpeg_toolbox.load('lena.jpg', False)
#pprint(r)
#print(r.keys())
#print(r["comp_info"])
#pprint(r["coef_arrays"])
#np.set_printoptions(threshold=sys.maxsize)
#print(r["coef_arrays"])

jpeg_toolbox.save(r, 'lena2.jpg')
r1 = jpeg_toolbox.load('lena2.jpg', False)


print(r1==r)

