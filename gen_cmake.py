import json
import os

db = json.load(open('compile_commands.json'))
includes_ordered, includes, defines, force_includes, sources = [], set(), set(), [], []

CHIBIOS_CONFIG_HEADERS = ['halconf.h', 'mcuconf.h', 'chconf.h']

first_entry = True
for entry in db:
    # Skip generated build files
    if not entry['file'].startswith('.build/'):
        sources.append(entry['file'])
    if not first_entry:
        continue
    first_entry = False
    args = entry['arguments']
    i = 1
    while i < len(args):
        a = args[i]
        if a.startswith('-I'):
            inc = a[2:] or (args[i+1] if i+1 < len(args) else '')
            if inc and inc not in includes:
                includes.add(inc)
                includes_ordered.append(inc)
        elif a == '-I' and i+1 < len(args):
            inc = args[i+1]
            if inc not in includes:
                includes.add(inc)
                includes_ordered.append(inc)
            i += 1
        elif a.startswith('-D'):
            val = a[2:]
            if val and not val.startswith('__') and not val.endswith('='):
                defines.add(val)
        elif a == '-include' and i+1 < len(args):
            path = args[i+1]
            if not path.startswith('.build/'):
                force_includes.append(path)
            i += 1
        i += 1

# Find ChibiOS config headers in include path order
found_configs = {}
for inc in includes_ordered:
    if inc.startswith('/') or inc.startswith('.build'):
        continue
    for header in CHIBIOS_CONFIG_HEADERS:
        if header not in found_configs:
            candidate = os.path.join(inc, header)
            if os.path.isfile(candidate):
                found_configs[header] = inc
                print('Found {}: {}/{}'.format(header, inc, header))

config_force_includes = [
    '{}/{}'.format(inc, h)
    for h, inc in found_configs.items()
]
all_force_includes = config_force_includes + force_includes

with open('CMakeLists.txt', 'w') as f:
    f.write('cmake_minimum_required(VERSION 3.20)\n')
    f.write('set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-toolchain.cmake)\n')
    f.write('project(qmk_tcy C)\n\n')
    f.write('include_directories(\n')
    for inc in includes_ordered:
        f.write('    ${{CMAKE_SOURCE_DIR}}/{}\n'.format(inc))
    f.write(')\n\nadd_compile_definitions(\n')
    for d in sorted(defines):
        f.write('    {}\n'.format(d.replace('"', '\\"')))
    f.write(')\n\n')
    if all_force_includes:
        f.write('# Force-included headers (ChibiOS configs + explicit -include flags)\n')
        f.write('add_compile_options(\n')
        for fi in all_force_includes:
            f.write('    "-include${{CMAKE_SOURCE_DIR}}/{}"\n'.format(fi))
        f.write(')\n\n')
    f.write('add_library(qmk_tcy\n')
    for src in sorted(set(sources)):
        f.write('    ${{CMAKE_SOURCE_DIR}}/{}\n'.format(src))
    f.write(')\n')

print('Done: {} includes, {} defines, {} force-includes ({} config headers), {} sources'.format(
    len(includes), len(defines), len(all_force_includes), len(config_force_includes), len(set(sources))))