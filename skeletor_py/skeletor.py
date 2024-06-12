#!/usr/bin/env python3

import os
import pyperclip
import time


def get_paths():

    dirs = []
    files = []

    for path, subdirs, filenames in os.walk("."):
        for subdir in subdirs:

            dirs.append(os.path.join(path, subdir))

        for filename in filenames:
            files.append(os.path.join(path, filename))

    return dirs, files


def get_unique_directories(dirs):
    unique_dirs = set(dirs)

    for dir_path in dirs:
        for other_path in dirs:
            if dir_path != other_path and dir_path.startswith(other_path):
                unique_dirs.discard(dir_path)
    return list(unique_dirs)


def build_mkdir_command(dirs):

    if not dirs:
        return ""
    return f'mkdir -p {" ".join(dirs)}'


def build_touch_command(files):
    if not files:
        return ""
    return f'touch {" ".join(files)}'


def copy_to_clipboard(text):

    try:
        pyperclip.copy(text)
        print("Copied to Clipboard")
    except pyperclip.PyperclipException as e:
        print(f"Failed to copy {e}")


def main():

    dirs, files = get_paths()
    unique_dirs = get_unique_directories(dirs)
    mkdir_command = build_mkdir_command(unique_dirs)
    touch_command = build_touch_command(files)

    combined_command = f"{mkdir_command} && {touch_command}".strip(" && ")
    copy_to_clipboard(combined_command)


if __name__ == "__main__":
    start_time = time.time()
    main()
    end_time = time.time()
    duration = end_time - start_time
    print(f"Taken: {duration:.4f} seconds")
