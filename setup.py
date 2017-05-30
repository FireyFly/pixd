from setuptools import setup

with open('README.rst') as readme_file:
    readme = readme_file.read()

setup(
    name='pixd',
    version='0.0.1',
    description='A tool for visualizing binary data using a colour palette',
    long_description=readme,
    author='Alex Willmer',
    author_email='alex@moreati.org.uk',
    url='https://github.com/moreati/python-pixd',
    py_modules=[
        'pixd',
    ],
    license="MIT license",
    zip_safe=True,
    keywords=[
        'hexdump',
    ],
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Environment :: Console',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
    ],
)
