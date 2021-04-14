""""
This python script creates a zip archive based on the built project. The zip 
archive is placed in a one drive folder, and versioning is done automatically.
"""

from zipfile import ZipFile
import datetime
import os
from os.path import basename
from pathlib import Path

# Some configurations
VERSION = "2"
IN_PATH = str(Path.home()) + "\\source\\repos\\Erovra3\\"
OUT_PATH = str(Path.home()) + "\\OneDrive\\Erovra versions\\"

# Counts how many previous copies of versions there are
def count_files(dir_path):
    count = 0
    for filename in os.listdir(dir_path):
        if filename.startswith("Erovra3." + VERSION + "."):
            count += 1
    return count

version_num = 0
filename = OUT_PATH + "Erovra3." + VERSION + "." + str(count_files(OUT_PATH)) + ".zip"

# Create archive file
with ZipFile(filename, 'w') as zip:
    # Add .exe file
    zip.write(IN_PATH + "x64\\Debug\\Erovra3.exe", "Erovra3.exe")
    # Add res dir and all files in res
    for res_filename in os.listdir(IN_PATH + "Erovra3\\res"):
        zip.write(IN_PATH + "Erovra3\\res\\" + res_filename, "res\\" + res_filename)
    # Add all .dll files
    for dll_filename in os.listdir(IN_PATH + "Erovra3\\"):
        if (dll_filename.endswith(".dll")):
            zip.write(IN_PATH + "Erovra3\\" + dll_filename, dll_filename)