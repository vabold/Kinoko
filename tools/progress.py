import json
import os
import requests
import struct
import sys

BADGE_URL_PREFIX = "https://badgen.net/static/"

OUT_BADGE_DIR = "out/website/"
OUT_BADGE_EXT = ".svg"

if __name__ == '__main__':
    # Look through provided Kinoko output
    lines = b''
    with open(sys.argv[1], "r") as f:
        lines = f.readlines();
    os.makedirs(os.path.dirname(OUT_BADGE_DIR), exist_ok=True)
    for name, sync, targetFrames, totalFrames in zip(*[iter(lines)]*4):
        name = name.strip()
        sync = int(sync)
        targetFrames = int(targetFrames)
        totalFrames = int(totalFrames)

        if (sync == 0):
            print(name + " desynced! Exiting out...")
            exit(1)
        
        percent = (targetFrames / totalFrames) * 100
        
        url = BADGE_URL_PREFIX + name + "/" + f"{percent:.1f}" + "%/green"
        r = requests.get(url)
        with open(OUT_BADGE_DIR + name + OUT_BADGE_EXT, 'wb') as f:
            f.write(r.content)
