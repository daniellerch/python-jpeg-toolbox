#!/usr/bin/env python3

import jpeg_toolbox
from pprint import pprint

jpg = jpeg_toolbox.load("image.jpg")
pprint(jpg)


jpg["coef_arrays"][0][0][0] = 85
jpeg_toolbox.save(jpg, 'lena2.jpg')

