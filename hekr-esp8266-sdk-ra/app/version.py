import re
import os
import time

def get_bin_version(bin_version_file) :
    # get bin ver
    bin_fp = open(bin_version_file, "r")
    bin_info = bin_fp.read()
    bin_info_ver = re.findall(r'#define BIN_VERSION "(.+?)"', bin_info, re.M)

	
    ver = bin_info_ver[0]
    # print ver
    time_str = time.strftime("%m%d%H%M", time.localtime())
    ver = ver +'build_' + time_str
    return ver

# if __name__ == '__main__':
    # print get()
