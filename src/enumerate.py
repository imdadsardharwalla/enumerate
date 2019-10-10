import argparse
import glob
import math
import os
import shutil

from stat import S_ISREG

START_AT_MIN = 0
START_AT_MAX = 2 ** 31 # Some very large number

def non_negative_int (value):
    ivalue = int (value)
    if (ivalue < START_AT_MIN or ivalue > START_AT_MAX):
        raise argparse.ArgumentTypeError (
            "{0} must be between {1} and {2} inclusive.".format (
                value, START_AT_MIN, START_AT_MAX))
    return ivalue


def get_mtime (file_property):
    return file_property["mtime"]


def get_ctime (file_property):
    return file_property["ctime"]


def get_name (file_property):
    return file_property["name"]


parser = argparse.ArgumentParser ("Enumerate a list of files")

sort_type_group = parser.add_mutually_exclusive_group ()
sort_type_group.add_argument ("-d", "--by-date", action="store_true",
                              help="Sort files by date created")
sort_type_group.add_argument ("-n", "--by-name", action="store_true",
                              help="Sort files by file name")

parser.add_argument ("-s", "--start-at", type=non_negative_int, default=1,
                     help="The number of the first enumerated file")
parser.add_argument ("-o", "--output-dir", type=str, default="./",
                     help="Directory in which the enumerated files should be placed")
parser.add_argument ("file_pattern", type=str, nargs="+",
                     help="(Wildcard) pattern to select files, e.g. '*.*', 'readme.txt' or '../*.png'")

args = parser.parse_args ()

sort_type = "NONE"
if (args.by_date):
    sort_type = "BY_DATE"
elif (args.by_name):
    sort_type = "BY_NAME"

# We expand the file patterns to get a list of existing paths
# (directories and files). If a pattern does not expand to an existing
# path, it will be ignored.
file_property_list = []
for pattern in  args.file_pattern:
    # Create a list of paths that match the pattern
    glob_pattern = glob.glob (pattern)

    # Then for each of these paths...
    for path in glob_pattern:
        stat_path = os.stat (path)

        # If the path is not a regular file, skip it
        if (not S_ISREG (stat_path.st_mode)):
            continue

        # Add the path, along with some of its properties to the list
        # of files to be enumerated
        file_property_list += [{
            "path" : path,
            "name" : os.path.basename (path),
            "mode" : stat_path.st_mode,
            "mtime": stat_path.st_mtime_ns,
            "ctime": stat_path.st_ctime_ns,
            "inode": stat_path.st_ino,
            "dev"  : stat_path.st_dev
        }]

# Sort the list of files according to the selected sort type
if (sort_type == "BY_DATE"):
    file_property_list.sort (key=get_ctime)
elif (sort_type == "BY_NAME"):
      file_property_list.sort (key=get_name)

# Calculate number of digits to use for enumeration
num_files = len (file_property_list)
num_digits = math.floor (math.log (num_files, 10)) + 1

# Rename the files one by one
count = args.start_at
for file_property in file_property_list:
    extension = os.path.splitext(file_property["path"])[1].strip ()
    extended_count = format (count, "0{0}d".format (num_digits))
    new_path = os.path.join (args.output_dir, extended_count + extension)

    shutil.move (file_property["path"], new_path)
    print ("({0}) {1}".format (extended_count, file_property["path"]))

    count += 1
