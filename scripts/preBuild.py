import json
import os
jsonfile = open('buildinfo.json', 'rb+')
build_info = json.loads(jsonfile.read())
jsonfile.close()

version = build_info['version']

Cfile = open('bsp/version.h','r')
content = Cfile.read()
Cfile.close()
if content.find(version.strip('v')) > 0:
    os._exit(0)

version_list = version.strip('v').split('.')
major,minor,internal = version_list
major = int(major)
minor = int(minor)
internal = int(internal)

fw_decimal = major*100+minor*10+internal
fw_version = hex( (((major<<4) | minor)<<8) | internal ) 
fw_version_byte = hex( ((major<<4) | minor) )
fw_vrsion_str = version.strip('v')


content = """#ifndef VERSION_H
#define VERSION_H

#define FW_DECIMAL      ({fw_decimal})
#define FW_VERSION      ({fw_version})
#define FW_VERSION_BYTE ({fw_version_byte})
#define FW_VERSION_STR  ("{fw_version_str}")

#endif
""".format(
    fw_decimal = fw_decimal,
    fw_version = fw_version,
    fw_version_byte = fw_version_byte,
    fw_version_str = fw_vrsion_str
    )

version_file = open('bsp/version.h','wb+')
version_file.write(content)
version_file.close()

