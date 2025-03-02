import os, sys
from setuptools import setup, Extension, find_packages


if sys.platform == "win32":
    extra_compile_args = ["/std:c++11"]
    extra_link_args = []
elif sys.platform == "darwin":
    extra_compile_args = ["-std=c++11", "-stdlib=libc++"]
    extra_link_args = []
else:
    extra_compile_args = ["-std=c++11", "-Wno-narrowing"]
    extra_link_args = []

jpeg_extension = Extension(
    "jpeg_toolbox.jpeg_extension",
    include_dirs=["pyjpeg/"],
    sources=["jpeg_toolbox/jpeg_toolbox_extension.c"],
    libraries=["jpeg"],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
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
      package_data={"jpeg_toolbox": 
        ["libs/libjpeg.so.9", "../LICENSE-libjpeg.txt"]},
      include_package_data=True,
      classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
      ],
      python_requires='>=3.6',
)



