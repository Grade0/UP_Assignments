"""
	name: Chen Davide
	student n.:19207819	
	email: davide.chen@ucdconnect.ie
"""

#!/usr/bin/python3
import getopt
import sys
import re
from datetime import datetime

def num_IP():
	IP_add_set = set()
	with open(sys.argv[1], "r") as FileIN:
		lines = FileIN.readlines()
		for line in lines:
			test = re.match("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}", line)
			if test:
				IP_add_set.add(test.group())
		print(len(IP_add_set))



def n_more_req(n):
	IP_add_dic = dict()
	with open(sys.argv[1], "r") as FileIN:
		lines = FileIN.readlines()
		for line in lines:
			test = re.match("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}", line)
			if test:
				if test.group() in IP_add_dic:
					IP_add_dic[test.group()] = IP_add_dic[test.group()] + 1
				else:
					IP_add_dic[test.group()] = 1
		sorted_dic = sorted(IP_add_dic.items(), key=lambda x: x[1], reverse=True)
		print ("{:20s} {:25s}".format("IP", "Requests"))
		for x in sorted_dic[0:n]:
			print ("{:20s} {:8d}".format(x[0], x[1]))

def get_num_vis(ip):
	IP_add_dic = dict()
	with open(sys.argv[1], "r") as FileIN:
		lines = FileIN.readlines()
		for line in lines:
			test = re.match("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}", line)
			if test:
				if test.group() in IP_add_dic:
					IP_add_dic[test.group()] = IP_add_dic[test.group()] + 1
				else:
					IP_add_dic[test.group()] = 1
		num_visit = IP_add_dic.get(ip)
		print(num_visit)


def get_list_req(ip):
	IP_add_dic = dict()
	with open(sys.argv[1], "r") as FileIN:
		lines = FileIN.readlines()
		for line in lines:
			test = re.match("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}", line)
			if test:
				req = test.group()
				if req == ip:
					print(line)

def get_date_req(date1):
	IP_add_dic = dict()
	with open(sys.argv[1], "r") as FileIN:
		lines = FileIN.readlines()
		for line in lines:
			test = re.match("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}", line)
			match1 = re.match('\d{2}\/\w{3}\/\d{4}', line)
			if match1:
				z = match1.group()
				print(z)

def main():
	try:
		options, args = getopt.getopt(sys.argv[2:], 'nt:v:L:d:')
	except getopt.GetoptError as err:
		print(err)
		sys.exit(2)

	print("Processing, please wait...\n")
	for opt, arg in options:
		if opt == "-n":
			num_IP()
		elif opt == "-t":
			n_more_req(int(arg))
		elif opt == "-v":
			get_num_vis(arg)
		elif opt == "-L":
			get_list_req(str(arg))
		elif opt == "-d":
			get_date_req(arg)


if __name__=="__main__":
	main()