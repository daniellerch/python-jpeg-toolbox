import os, sys
from setuptools import setup, Extension, find_packages
from distutils.command.build_ext import build_ext as build_ext_orig

class CTypesExtension(Extension): pass
class build_ext(build_ext_orig):

    def build_extension(self, ext):
        self._ctypes = isinstance(ext, CTypesExtension)
        return super().build_extension(ext)

    def get_export_symbols(self, ext):
        if self._ctypes:
            return ext.export_symbols
        return super().get_export_symbols(ext)

    def get_ext_filename(self, ext_name):
        if self._ctypes:
            return ext_name + '.pyd'
        return super().get_ext_filename(ext_name)



if sys.platform == "win32":

    cmdclass={'build_ext': build_ext}

    package_data={"jpeg_toolbox": 
        ["jpeg-9c-win/Lib/static_x64/jpeg.lib", 
         "../LICENSE-libjpeg.txt"]}

    jpeg_extension = CTypesExtension(
        "jpeg_toolbox.jpeg_extension",
        sources = ['jpeg_toolbox/jpeg_toolbox_extension.c'], 
        include_dirs = ['jpeg_toolbox/jpeg-9c-win/Include/'],
        libraries = ['jpeg_toolbox/jpeg-9c-win/Lib/static_x64/jpeg'],
        library_dirs=['jpeg_toolbox/jpeg-9c-win/Lib/static_x64/'],
        extra_link_args=[f'/DEFAULTLIB:jpeg_toolbox/jpeg-9c-win/Lib/static_x64/jpeg.lib'],
    )

elif sys.platform == "darwin":
    cmdclass={}

    package_data={"jpeg_toolbox": 
        ["libs/libjpeg.so.9", "../LICENSE-libjpeg.txt"]}

    jpeg_extension = Extension(
        "jpeg_toolbox.jpeg_extension",
        include_dirs=["pyjpeg/"],
        sources=["jpeg_toolbox/jpeg_toolbox_extension.c"],
        libraries=["jpeg"],
        extra_compile_args= ["-std=c++11", "-stdlib=libc++"],
        extra_link_args=[],
        language="c++",
    )

else:
    cmdclass={}

    package_data={"jpeg_toolbox": 
        ["libs/libjpeg.so.9", "../LICENSE-libjpeg.txt"]}

    jpeg_extension = Extension(
        "jpeg_toolbox.jpeg_extension",
        include_dirs=["pyjpeg/"],
        sources=["jpeg_toolbox/jpeg_toolbox_extension.c"],
        libraries=["jpeg"],
        extra_compile_args=["-std=c++11", "-Wno-narrowing"],
        extra_link_args=[],
        language="c++",
    )




setup(name = 'python-jpeg-toolbox',
      version = '0.1.0',
      author="Daniel Lerch Hostalot",
      author_email="dlerch@gmail.com",
      url="https://github.com/daniellerch/python-jpeg-toolbox",
      description = 'The JPEG Toolbox for Python',
      long_description=open("README.md").read(),
      long_description_content_type="text/markdown",
      install_requires=[
        "numpy",
      ],
      packages=find_packages(),
      ext_modules = [jpeg_extension],
      cmdclass={'build_ext': build_ext},
      package_data = package_data,
      include_package_data=True,
      classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
      ],
      python_requires='>=3.6',
)



