#!/usr/bin/env python3
"""Bundle a CMake-built macOS app and its non-system dynamic libraries."""
import argparse
import json
from pathlib import Path
import plistlib
import shutil
import subprocess


def output(*cmd):
    return subprocess.check_output([str(x) for x in cmd], text=True)


def run(*cmd):
    subprocess.run([str(x) for x in cmd], check=True)


def system(path):
    return path.startswith(('/usr/lib/', '/System/Library/'))


def dependencies(binary):
    return [line.strip().split(' (compatibility version', 1)[0]
            for line in output('otool', '-L', binary).splitlines()[1:] if line.strip()]


def rpaths(binary):
    lines = output('otool', '-l', binary).splitlines()
    return [lines[i + 2].strip().split('path ', 1)[1].split(' (offset', 1)[0]
            for i, line in enumerate(lines) if line.strip() == 'cmd LC_RPATH']


def minimum_macos(binary):
    command = ""
    versions = []
    for line in output('otool', '-l', binary).splitlines():
        parts = line.split()
        if len(parts) == 2 and parts[0] == 'cmd':
            command = parts[1]
        if len(parts) == 2 and ((command == 'LC_BUILD_VERSION' and parts[0] == 'minos')
                              or (command == 'LC_VERSION_MIN_MACOSX' and parts[0] == 'version')):
            versions.append(tuple(int(n) for n in parts[1].split('.')))
    return max(versions, default=(10, 15))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--app', required=True, type=Path, help='CMake-built perimeter.app')
    parser.add_argument('--output', required=True, type=Path, help='new destination .app (must not exist)')
    parser.add_argument('--icd', required=True, type=Path, help='MoltenVK ICD JSON installed on the build host')
    parser.add_argument('--content', type=Path, help='optional complete game directory to copy for personal use')
    parser.add_argument('--library-dir', action='append', default=[], type=Path)
    args = parser.parse_args()
    source_app = args.app.resolve()
    dest = args.output.absolute()
    if dest.exists() or dest.suffix != '.app':
        parser.error('--output must be a new .app path')
    if source_app in dest.resolve().parents:
        parser.error('--output must be outside the source app')
    if args.content and (args.content.resolve() == dest.resolve() or args.content.resolve() in dest.resolve().parents):
        parser.error('--output must be outside the content directory')
    executable = source_app / 'Contents/MacOS/perimeter'
    if not executable.is_file():
        parser.error('expected Contents/MacOS/perimeter in --app')
    if args.content and not args.content.is_dir():
        parser.error('--content must be a game directory')
    icd = json.loads(args.icd.read_text())
    molten = (args.icd.parent / icd['ICD']['library_path']).resolve()
    if not molten.is_file():
        parser.error('MoltenVK ICD library was not found')

    # Resolve the complete graph before creating the output. Missing libraries
    # fail packaging instead of silently leaving build-host paths in the app.
    graph = {}
    names = {}
    inherited = rpaths(executable)
    def expand(value, owner):
        return value.replace('@loader_path', str(owner.parent)).replace('@executable_path', str(executable.parent))
    def resolve(dep, owner):
        if dep.startswith('@rpath/'):
            suffix = dep[len('@rpath/'):]
            choices = [Path(expand(p, owner)) / suffix for p in rpaths(owner)]
            choices += [Path(expand(p, executable)) / suffix for p in inherited]
            choices += [p.resolve() / suffix for p in args.library_dir]
        else:
            choices = [Path(expand(dep, owner))]
        for p in choices:
            if p.is_file():
                return p.resolve()
        raise RuntimeError(f'Cannot resolve {dep} required by {owner}')
    def visit(binary):
        binary = binary.resolve()
        if binary in graph:
            return
        if binary != executable.resolve():
            previous = names.setdefault(binary.name, binary)
            if previous != binary:
                raise RuntimeError(f'Conflicting library names: {previous} and {binary}')
        graph[binary] = {}
        ids = output('otool', '-D', binary).splitlines()[1:]
        for dep in dependencies(binary):
            if dep in ids or system(dep):
                continue
            child = resolve(dep, binary)
            graph[binary][dep] = child
            visit(child)
    visit(executable)
    visit(molten)
    shutil.copytree(source_app, dest)
    frameworks = dest / 'Contents/Frameworks'
    frameworks.mkdir(exist_ok=True)
    resources = dest / 'Contents/Resources'
    resources.mkdir(exist_ok=True)
    if args.content:
        shutil.copytree(args.content, resources / 'Content')
    target_exe = dest / 'Contents/MacOS/perimeter'
    for source, deps in graph.items():
        is_exe = source == executable.resolve()
        target = target_exe if is_exe else frameworks / source.name
        if not is_exe:
            shutil.copy2(source, target)
            target.chmod(target.stat().st_mode | 0o200)
            run('install_name_tool', '-id', '@rpath/' + target.name, target)
        for old, child in deps.items():
            prefix = '@executable_path/../Frameworks/' if is_exe else '@loader_path/'
            run('install_name_tool', '-change', old, prefix + child.name, target)
        for path in rpaths(target):
            run('install_name_tool', '-delete_rpath', path, target)
    icd['ICD']['library_path'] = '../Frameworks/' + molten.name
    (resources / 'MoltenVK_icd.json').write_text(json.dumps(icd, indent=2) + '\n')
    launcher = dest / 'Contents/MacOS/start'
    launcher.write_text('''#!/bin/sh
set -eu
macos_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
export VK_DRIVER_FILES="$macos_dir/../Resources/MoltenVK_icd.json"
export VK_ICD_FILENAMES="$VK_DRIVER_FILES"
if [ -d "$macos_dir/../Resources/Content" ]; then
    exec "$macos_dir/perimeter" "content=$macos_dir/../Resources/Content" "$@"
fi
exec "$macos_dir/perimeter" "$@"
''')
    launcher.chmod(0o755)
    plist_path = dest / 'Contents/Info.plist'
    with plist_path.open('rb') as f:
        plist = plistlib.load(f)
    plist['CFBundleExecutable'] = 'start'
    declared_min = tuple(int(n) for n in plist.get('LSMinimumSystemVersion', '10.15').split('.'))
    required_min = max([declared_min, *(minimum_macos(binary) for binary in graph)])
    plist['LSMinimumSystemVersion'] = '.'.join(str(n) for n in required_min)
    with plist_path.open('wb') as f:
        plistlib.dump(plist, f)
    # Sign inside out after all install-name changes. This is a local ad-hoc
    # signature; it is not Developer ID signing or notarization.
    for lib in sorted(frameworks.iterdir()):
        if lib.is_file():
            run('codesign', '--force', '--sign', '-', lib)
    run('codesign', '--force', '--sign', '-', target_exe)
    run('codesign', '--force', '--sign', '-', dest)
    run('codesign', '--verify', '--deep', '--strict', dest)
    for binary in [target_exe, *frameworks.iterdir()]:
        if binary.is_file():
            for dep in dependencies(binary):
                if not system(dep) and not dep.startswith(('@loader_path/', '@executable_path/', '@rpath/')):
                    raise RuntimeError(f'Unbundled dependency: {binary}: {dep}')
    print(f'Bundled {len(names)} libraries into {dest}; minimum macOS {plist["LSMinimumSystemVersion"]}')


if __name__ == '__main__':
    main()
