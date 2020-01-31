# Bindings for OpenAir's implementation of LDPC

TBD: Long description

## Creating packages

```PowerShell
python setup.py sdist
pip wheel -w dist --verbose .
```

## Source code checking

```PowerShell
pylint OpenAirLDPC --rcfile .\pylint.conf
```