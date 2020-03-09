#!/usr/bin/python3

import sys
import numpy as np
from pprint import pprint
import jpeg_toolbox

r1 = jpeg_toolbox.load('lena.jpg')

jpeg_toolbox.save(r1, 'lena2.jpg')
r2 = jpeg_toolbox.load('lena2.jpg')



pprint(r1["coef_arrays"])
pprint(r2["coef_arrays"])


