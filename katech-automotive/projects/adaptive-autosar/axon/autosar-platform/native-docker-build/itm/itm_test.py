#!/usr/bin/env python3
# vim: set expandtab ts=2:
import json
import pprint
import os
import sys

json_obj = json.loads(open("/mnt/native-docker-build/itm/itm_freerun_testcases.json", "r").read())
logdir = "/var/redirected"

_FREERUN_COMPONENTS = {
    str(comp): False
    for comp in list(json_obj["FREERUN_PATTERN"].keys())
}
_FREERUN_PATTERN = {
    str(_com): [[False, str(pattern)] for pattern in patterns]
    for _com, patterns in list(json_obj["FREERUN_PATTERN"]
                                .items())
}
contents=""
for file in os.listdir(logdir):
    with open(os.path.join(logdir, file),"r") as f:
        contents += f.read()

_freerun_pass = True
for _com, _sucess in _FREERUN_COMPONENTS.items():
    # print(_com,_sucess)
    _com_pass = True
    for _i, _pattern in enumerate(_FREERUN_PATTERN[_com]):
        # print(_i,_pattern)
        if not _pattern[0]:
            if _pattern[1] in contents.strip():
                _FREERUN_PATTERN[_com][_i][0] = True
            else:
                _com_pass = False
                _freerun_pass = False
                print(_pattern[1]+" Failed")
    if _com_pass:
        _FREERUN_COMPONENTS[_com] = True

pprint.pprint(_FREERUN_PATTERN)
print("Freerun:", _freerun_pass)
if (_freerun_pass):
    sys.exit(0)
else:
    sys.exit(-1)
