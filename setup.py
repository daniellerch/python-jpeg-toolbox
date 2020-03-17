import os
from distutils.core import setup, Extension

m = Extension('jpeg_toolbox_extension', 
              sources = ['jpeg_toolbox_extension.c'], 
              libraries = ['jpeg'])

here = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(here, 'README.md'), encoding='utf-8') as f:
        long_description = f.read()

setup(name = 'jpeg-toolbox',
      version = '1.0',
      author="Daniel Lerch",
      author_email="dlerch@gmail.com",
      url="https://github.com/daniellerch/python-jpeg-toolbox",
      description = 'The JPEG Toolbox for Python',
      py_modules = ["jpeg_toolbox"],
      ext_modules = [m])
