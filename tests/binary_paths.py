#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 CNRS
# SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
"""The binaries of a package carry no path of the machine that built them.

usage: python3 tests/binary_paths.py <file or folder> ...

Reads the printable strings of every Mach-O, ELF or PE file given or found
under the folders, as strings(1) does, and fails on any that contains the
source folder of this repository or the home folder of the user who runs it:
a user name and a folder layout that had nothing to do in a package.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HOME = os.path.expanduser('~')
MAGIC = (b'\xcf\xfa\xed\xfe', b'\xce\xfa\xed\xfe', b'\xca\xfe\xba\xbe', b'\x7fELF', b'MZ')


def binaries(paths):
    for p in paths:
        if os.path.isdir(p):
            for d, _, files in os.walk(p):
                for f in files:
                    yield os.path.join(d, f)
        else:
            yield p


def found(path):
    with open(path, 'rb') as f:
        data = f.read()
    if not data.startswith(MAGIC):
        return []
    needles = {ROOT.encode(), HOME.encode() + b'/'}
    out = []
    for m in re.finditer(rb'[\x20-\x7e]{6,}', data):
        if any(n in m.group(0) for n in needles):
            out.append(m.group(0).decode('ascii', 'replace'))
    return out


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    bad = 0
    for path in binaries(sys.argv[1:]):
        if os.path.islink(path) or not os.path.isfile(path):
            continue
        strings = found(path)
        if strings:
            bad += len(strings)
            print(f'{path}: {len(strings)} strings with a path of this machine, e.g. {strings[0][:120]}')
    print(f'RESULT: {"FAIL" if bad else "PASS"} ({bad} strings)')
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
