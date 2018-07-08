#!/usr/bin/env python
# coding=utf-8
import sys
import os
import time
import version
	
bin_version_file_dir= 'user/version.h'

product_modle = []

def main():
	global product_moudle
	bin_description = ''
	if len(sys.argv) == 2:
		single_moudle = [sys.argv[1]]
		product_moudle = single_moudle
	if len(sys.argv) == 3:
		single_moudle = [sys.argv[1]]
		product_moudle = single_moudle
		bin_description = '\(' + sys.argv[2] + '\)'	
	try:
		bin_version = version.get_bin_version(bin_version_file_dir)
		product_func_bin = '../../product-bin/4.x-firmware/'+product_moudle[0]+'_Firmware_v'+bin_version+'/'
	except type, reason:
		print reason
	print 'deletecommand'
	deletecommand = 'rm -rf ../bin/upgrade/1.bin ../bin/upgrade/2.bin'

	print 'make_command'
	make_command = 'make clean && make APP=1 PRODUCT=-D__%s__' \
				   '&& make clean &&make APP=2 PRODUCT=-D__%s__' % (
                           product_moudle[0], product_moudle[0])
				   
	mkdir_command = 'mkdir -p '+product_func_bin
	
	copycommand = ' cp ../bin/upgrade/1.bin %s%s_Firmware_v%sbeta-1.bin' \
				  '&&cp ../bin/upgrade/2.bin %s%s_Firmware_v%sbeta-2.bin' % (
					  product_func_bin,
					  product_moudle[0],
					  bin_version+bin_description,
					  product_func_bin,
					  product_moudle[0],
					  bin_version+bin_description
					  )

	comand=[
			deletecommand,
			make_command,
			mkdir_command,
			copycommand
			]
	for n in range(len(comand)):
		try:
			ret = os.system(comand[n])
			if ret == 2:
				return ret
		except type, reason:
			print 'command=%s\nerror: %s' % (comand[n], reason)
			return -1
			
	print 'make all bin finish'


if __name__ == '__main__':
	main()
