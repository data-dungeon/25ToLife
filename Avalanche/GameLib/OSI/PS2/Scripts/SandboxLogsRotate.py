#
# Dirty little python script to rotate log files on RV server.
#
# Appends ISO-8601 time suffix to log files, so when server is restarted,
# new log files will be created.
#
# Author: Dan Kamins <dcon@axonchisel.net> | <dkamins@crystald.com>
#

import os
import datetime
import sys

# Get env params:
sbname = os.environ['SANDBOX_NAME']
sbpass = os.environ['SANDBOX_PASS']
sbpass = 'zopchG47&'  # override environment val, which is getting ^-quoted incorrectly!
winscp_dir = os.environ['WINSCP_DIR']

# Set up params:
logfiles = ["MySandbox","Quazal Rendez-Vous","QuazalAuthentication"]
suffix = datetime.datetime.now().strftime("%Y-%m-%dT%H-%M-%S")

# Build SFTP commands:
fileCmds = open("SandboxCommands.tmp", "w")
fileCmds.write("open sftp://ttl:%s@sftp.quazal.net:9200 \n" % (sbpass))
fileCmds.write("cd %s/Logs \n" % (sbname))
for logfile in logfiles:
	fileCmds.write("mv \"%s.log\" \"%s-%s.log\" \n" % (logfile, logfile, suffix) )
	print "ROTATING log file: \"%s.log\" -> \"%s-%s.log\"" % (logfile, logfile, suffix)
fileCmds.write("exit \n")
fileCmds.close()

# Do it:
print "*** SANDBOX MUST BE STOPPED TO ROTATE LOG FILES! ***"
os.system(winscp_dir + ' /console /script=SandboxCommands.tmp')
