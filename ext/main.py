import argparse
import sys
from os import path

parser = argparse.ArgumentParser(description='pyswmm6 tests.')
parser.add_argument('path', metavar='PATH', type=str,
                    help='path to search for pyswmm6')

if '__main__' == __name__:
    pypath = parser.parse_args().path
    if pypath is not None:
        abs_path = path.abspath(pypath)
        print("Using path {}".format(abs_path))
        sys.path.append(abs_path)
    else:
        print()
        sys.exit(1)
    from pyswmm6 import say_hello_to
    say_hello_to("Reese")
    sys.exit(0)
