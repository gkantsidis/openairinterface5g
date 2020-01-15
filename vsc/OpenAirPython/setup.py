import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="OpenAirLDPC", # Replace with your own username
    version="0.0.1",
    author="Christos Gkantsidis",
    author_email="chrisgk@microsoft.com",
    description="Exposes to Python an interface to the OpenAir's implementation of LDPC",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/gkantsidis/openairinterface5g",
    packages=setuptools.find_packages(exclude=['OpenAirLDPC.test']), #['OpenAirLDPC'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OAI Public License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
    package_data={'OpenAirLDPC': ['.libs/*']},
    exclude_package_data={'OpenAirLDPC': ['test_*.py']},
)
