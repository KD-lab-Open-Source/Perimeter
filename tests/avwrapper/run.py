#!/usr/bin/env python3
"""Standalone regression test. Requires a C++17 compiler, pkg-config, FFmpeg and SDL2 headers."""
import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--pevents-include', required=True, type=Path, help='directory containing pevents.h')
parser.add_argument('--sanitize', action='store_true')
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]

def run(cmd):
    subprocess.run([str(x) for x in cmd], check=True)

def pkg(*args):
    return shlex.split(subprocess.check_output(['pkg-config', *args], text=True))

with tempfile.TemporaryDirectory(prefix='perimeter-avwrapper-') as tmp:
    tmp = Path(tmp)
    flags = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer'] if args.sanitize else []
    run([os.environ.get('CXX', 'c++'), '-std=c++17', '-DPERIMETER_EXODUS', '-DPERIMETER_FFMPEG',
         '-DPERIMETER_FFMPEG_MOVIE', '-DNASSERT', *flags,
         '-I' + str(root / 'Source/XTool'), '-I' + str(root / 'Source/Util'),
         '-I' + str(args.pevents_include.resolve()), *pkg('--cflags', 'sdl2'),
         root / 'tests/avwrapper/regression.cpp', root / 'Source/Util/AVWrapper.cpp',
         *pkg('--cflags', '--libs', 'libavcodec', 'libavformat', 'libavutil', 'libswscale', 'libavfilter'),
         '-o', tmp / 'regression'])
    run(['ffmpeg', '-v', 'error', '-f', 'lavfi', '-i', 'testsrc=size=65x33:rate=30:duration=1',
         '-f', 'lavfi', '-i', 'sine=frequency=440:sample_rate=44100:duration=1',
         '-c:v', 'ffv1', '-pix_fmt', 'bgr0', '-c:a', 'pcm_s16le', tmp / 'fixture.mkv'])
    run([tmp / 'regression', tmp / 'fixture.mkv'])
