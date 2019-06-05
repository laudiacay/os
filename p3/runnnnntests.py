import util, commands, os, time

port = '10000'
host = 'localhost'
ctimeout = 5

testcase = 42
def uuuugh():
    rc = util.make()
    if rc == -1:
        return -1
    rc = util.compile_testclient(testcase)
    if rc == -1:
        return -1
    clientout = "out.txt"
    commands.getoutput("rm -rf out.txt")
    os.system("./testclient %s %s > %s &" % (port, host, clientout))
    time.sleep(ctimeout/2)
    rc = util.start_fresh_server(port)
    if rc == -1:
        return -1
    time.sleep(ctimeout)
    ps = commands.getoutput("ps -ax")
    if ps.find("./testclient %s %s" % (port, host)) >= 0:
        # the testclient is still running
        print "ERROR: your client did not implement timeout correctly"
        print "ERROR: testclient should have finished"
        return -1
    else: # now testclient finish, let check the return status
        cat = commands.getoutput("cat %s" % clientout)
        if cat.find("PASSED") >= 0:
            return 0
        return -1
print uuuugh()
