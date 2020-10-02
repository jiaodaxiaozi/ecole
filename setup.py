import sys
from setuptools import find_packages

from skbuild import setup


install_requires = ["numpy>=1.4"]
if sys.version_info.minor <= 6:
    install_requires += ["typing_extensions"]

setup(
    name="ecole",
    author="Antoine Prouvost",
    version="0.2.0",
    url="https://www.ecole.ai",
    description="Extensible Combinatorial Optimization Learning Environments",
    license="BSD-3-Clause",
    packages=find_packages("python/src"),
    package_dir={"": "python/src"},
    package_data={"ecole": ["py.typed"]},
    cmake_languages=["CXX"],
    cmake_install_dir="python/src",
    zip_safe=False,
    python_requires=">=3.6",
    install_requires=install_requires,
)
