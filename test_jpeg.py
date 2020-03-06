#!/usr/bin/python3

from pprint import pprint
from ctypes import *

def load(path):
   jpeg = CDLL('./jpeg.so')
   jpeg.read_file.restype = py_object
   r = jpeg.read_file(path.encode())
   return r


r = load('lena.jpg')
pprint(r)

