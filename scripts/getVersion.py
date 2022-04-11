#!/usr/bin/env python

import json

jsonfile = open('buildinfo.json', 'rb+')
build_info = json.loads(jsonfile.read())
jsonfile.close()

print (build_info['version'])