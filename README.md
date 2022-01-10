# WBE-C
C implementation of toy web-browser from https://browser.engineering/

## Rationale
I want to implement something meaty in C, in order to get my head around it well for work. This project offers to be 
perfect; it deals with networking (incl. SSL), external libraries (Tk), complex multi-part build chains, and actually
offers something of use.

Of course, I am not anticipating that the toy implementation I create will be replacing Firefox any time soon 😁.

Unless otherwise noted, this is a straight port of the Python code in https://github.com/browserengineering/book to C, and 
this has been done by me.

## Usage
To install and use the 'browser', do the following:

```
make clean
make
```

The browser can be run by passing the URL to the 'browser' executable as an option (-u), or by itself (e.g. `./browser http://example.com/`).

SSL support can be configured via the -s flag, and verbose mode via the -v flag. Any other options will print a help message
and then exit.
