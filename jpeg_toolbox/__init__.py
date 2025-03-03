import os
import ctypes
import glob
from pathlib import Path

module_dir = Path(__file__).parent

libjpeg_path = module_dir / "libs" / "libjpeg.so.9"
if os.name != "nt":
    try:
        if libjpeg_path.exists():
            ctypes.CDLL(str(libjpeg_path))
    except OSError as e:
        raise ImportError(f"Failed to load libjpeg: {e}")

shared_libs = glob.glob(str(module_dir / "jpeg_extension*.so"))
if not shared_libs and os.name == "nt":
    shared_libs = glob.glob(str(module_dir / "jpeg_extension*.pyd"))

if not shared_libs:
    raise ImportError("jpeg_extension not found. Make sure the package is installed correctly.")

try:
    jpeg_toolbox = ctypes.CDLL(shared_libs[0])
except OSError as e:
    raise ImportError(f"Failed to load jpeg_extension: {e}")

from .jpeg_toolbox import load, save


