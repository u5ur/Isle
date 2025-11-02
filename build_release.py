#!/usr/bin/env python3
import os
import subprocess
import sys

def run(cmd):
    print(f"> {' '.join(cmd)}")
    subprocess.run(cmd, check=True)

def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    print("Building Isle (Release)")
    run(["cmake", "--preset", "x64-release"])
    run(["cmake", "--build", "--preset", "build-release"])
    print("Build complete")

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print(f"Build failed with exit code {e.returncode}")
        sys.exit(e.returncode)
