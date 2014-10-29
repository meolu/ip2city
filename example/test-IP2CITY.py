#! /usr/bin/env python
#coding:u8
""" IPLocator: locate IP in the QQWry.dat.
    Usage:
        python IPLocator.py <ip>
    Create and test with Python 2.2.3.
    spadger@bmy <echo.xjtu@gmail.com> 2008-2-19
"""

import IP2CITY

#Demo
def main():
    IPL = IP2CITY.IPLocator( "UTFWry.dat" )
    ip = ""
    if len(sys.argv) != 2:
        #print 'Usage: python IPLocator.py <IP>'
        #return
        ip = "i121.199.24.143"
    else:
        ip = sys.argv[1]

    print IPL.str2ip(ip)
    address = IPL.getIpAddr( IPL.str2ip(ip) )
    range = IPL.getIpRange( IPL.str2ip(ip) )
    print "此IP %s 属于 %s\n所在网段: %s" % (ip,address, range)


if __name__ == "__main__" :
    main()
