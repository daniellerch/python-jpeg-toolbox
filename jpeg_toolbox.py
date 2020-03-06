#!/usr/bin/python3

from ctypes import *
import numpy as np

def load(path, use_numpy=True):
   jpeg = CDLL('./jpeg.so')
   jpeg.read_file.restype = py_object
   r = jpeg.read_file(path.encode())

   if not use_numpy:
      return r

   r["quant_tables"] = np.array(r["quant_tables"])
   r["coef_arrays"] = np.array(r["coef_arrays"])

   for i in range(len(r["ac_huff_tables"])):
      r["ac_huff_tables"][i]["counts"] = np.array(r["ac_huff_tables"][i]["counts"])
      r["ac_huff_tables"][i]["symbols"] = np.array(r["ac_huff_tables"][i]["symbols"])

   for i in range(len(r["dc_huff_tables"])):
      r["dc_huff_tables"][i]["counts"] = np.array(r["dc_huff_tables"][i]["counts"])
      r["dc_huff_tables"][i]["symbols"] = np.array(r["dc_huff_tables"][i]["symbols"])

   new_coefs = []
   for i in range(len(r["coef_arrays"])):
      tmp = r["coef_arrays"][i].reshape((64,64,8,8))
      tmp = np.moveaxis(tmp, [0,1,2,3],[0,2,1,3])
      tmp = tmp.reshape((512,512))
      new_coefs.append(tmp)

   r["coef_arrays"] = np.array(new_coefs)

   return r


