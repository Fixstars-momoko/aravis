import io

from setuptools import setup
import sys
from typing import List
import platform
import sysconfig
import os
from setuptools._distutils.util import convert_path

def package_files(directorys):
    paths = []
    for directory in directorys:
        for (path, directories, filenames) in os.walk(directory):
            for filename in filenames:
                paths.append(os.path.join('..', path, filename))
    return paths

def get_plat():
    if platform.system() == 'Linux':
        plat_form = "manylinux1_x86_64"
    else:
        plat_form = sysconfig.get_platform()
    return plat_form


def get_version():
    if os.environ.get("ARAVIS_VER") is not None:
        tag = os.environ.get("ARAVIS_VER")
        if tag.startswith('v'):
            tag=tag[1:]
        elif tag.startswith('v.'):
            tag=tag[2:]
    else:
        main_ns = {}
        ver_path = convert_path('./aravis/version.py')
        with open(ver_path) as ver_file:
            exec(ver_file.read(), main_ns)
        tag = main_ns["__version__"]
    return tag

def main():
    long_description = io.open("README.md", encoding="utf-8").read()
    package_data: List[str] = []

    extra_files = []
    if platform.system() == 'Windows':
        extra_files = package_files(['aravis/bin', 'aravis/lib'])
    elif platform.system() == 'Darwin':
        extra_files = package_files(['aravis/lib'])
    elif platform.system() == 'Linux':
        extra_files = package_files(['aravis/lib'])
    package_data = extra_files

    setup(
        name="aravis-python",
        version=get_version(),
        long_description=long_description,
        long_description_content_type="text/markdown",
        python_requires=">=3.9.0",
        packages=["aravis"],
        classifiers=[
            "Development Status :: 3 - Alpha",
            "Programming Language :: Python :: 3",
            "Programming Language :: Python :: 3 :: Only",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: Python :: 3.11",
            "Programming Language :: Python :: 3.12",
        ],

        description="Python Binding for aravis",
        package_data={"aravis": package_data},
        install_requires=[],
        # ext_modules=EmptyListWithLength(),
        include_package_data=False,
        options={
            "bdist_wheel": {
                "plat_name": get_plat(),
                "python_tag": "py3",
            },
        },
        license = "GNU General Public License"
    )


# This creates a list which is empty but returns a length of 1.
# Should make the wheel a binary distribution and platlib compliant.
class EmptyListWithLength(list):
    def __len__(self):
        return 1


if __name__ == "__main__":
    main()
