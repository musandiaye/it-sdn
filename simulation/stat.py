# -*- coding: utf-8 -*-
"""
Created on Tue Sep 19 16:45:41 2017

@author: musa
"""

import re, sys
from collections import defaultdict

packetTypes = {
0: 'SDN_PACKET_CONTROL_FLOW_SETUP',
1: 'SDN_PACKET_DATA_FLOW_SETUP',
2: 'SDN_PACKET_CONTROL_FLOW_REQUEST',
3: 'SDN_PACKET_DATA_FLOW_REQUEST',
4: 'SDN_PACKET_NEIGHBOR_REPORT',
5: 'SDN_PACKET_DATA',
6 + 0xE0 : 'SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP',
7 + 0xE0: 'SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP',
8 + 0xD0: 'SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP',
9 + 0xD0: 'SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP',
10: 'SDN_PACKET_ND',
11: 'SDN_PACKET_CD',
12: 'SDN_PACKET_ACK',
13: 'SDN_PACKET_REGISTER_FLOWID'
}

tx = defaultdict(list)
rx = defaultdict(list)

expression = re.compile(r"""
    (?P<TIME>.*) # First field is time, we do not match any pattern
    (?:\s.*ID:|:) # ID field is different in Cooja GUI and command line (ID: or just a :)
    (?P<ID>\d+) # the actual id is a sequence of numbers
    (?:\s|:).* # finishing id pattern (colon or space) and matching everything else until getting what we want
    =\[\ (?P<ADDR>[0-9A-F][0-9A-F]\ [0-9A-F][0-9A-F])\ \] # address field
    =(?P<RTX>RX|TX) # RX or TX
    =(?P<TYPE>[0-9A-F][0-9A-F]) # messageType
    =(?P<SEQNUM>[0-9A-F][0-9A-F]) # sequence number
    =(?P<DEST>.*)= # destination
    """, re.X) #re.X: verbose so we can comment along

# time expression (miliseconds or H+:MM:SS:miliseconds)
timeExpr = re.compile("(?:(\d+)?:?(\d\d):(\d\d).)?(\d+)")

# times_test = ("01:27.294", "01:11:37.434", "99999")
#
# for t in times_test:
#     s = timeExpr.match(t)
#     print t, ":", s.groups() if s!=None else "no match"

# f = ['5551598:2:~[~=[ 01 00 ]=TX=E7=01==']
f = file(sys.argv[1])

for l in f:
    # print l.strip()
    s = expression.search(l)
    if s != None:
        # print s.groups()
        time, printid, srcaddr, rxtx, messageType, seqNum, dest = s.groups()

        # print time
        # print printid
        # print srcaddr
        # print rxtx
        # print messageType,
        # print seqNum
        # print dest

        timeMatch = timeExpr.match(time)
        # print timeMatch
        timeMatch = map(lambda(x):float(x) if x != None else 0, timeMatch.groups())
        # print timeMatch
        time = 0
        time += timeMatch[0] * 1000 * 60 * 60 if timeMatch[0] != None else 0
        time += timeMatch[1] * 1000 * 60 if timeMatch[1] != None else 0
        time += timeMatch[2] * 1000 if timeMatch[2] != None else 0
        time += timeMatch[3]

        key = (srcaddr, messageType, seqNum, dest)
        if rxtx == "TX":
            tx[key] += [time]
        if rxtx == "RX":
            if len(tx[key]) > len(rx[key]):
                while len(tx[key]) > len(rx[key]) + 1:
                    rx[key] += [None]
                rx[key] += [time]
            elif len(tx[key]) == len(rx[key]):
                print "Chave repetida RX", key, "delta t:", time - rx[key][-1]
            else:
                print "not expected"


sent_type = defaultdict(int)
sent_node = defaultdict(int)

received_type = defaultdict(int)
received_node = defaultdict(int)

delay_type = defaultdict(float)
delay_node = defaultdict(float)

for k,v in tx.iteritems():
    sent_node[k[0]] += 1
    sent_type[k[1]] += 1
    for (tx_time, rx_time) in zip(v, rx[k]):
        if rx_time != None:
            received_node[k[0]] += 1
            received_type[k[1]] += 1
            delay_node[k[0]] += (rx_time - tx_time)
            delay_type[k[1]] += (rx_time - tx_time)
            if ((rx_time - tx_time) < 0):
                print "negative time", k, v

print "Delivery rates by source node:"
for node in sent_node.keys():
    print '(' + node + ')', "%.2f" % (100.0 * received_node[node] / sent_node[node],), '[', received_node[node], '/', sent_node[node], ']'

print
print "Delay by source node:"
for node in sent_node.keys():
    if received_node[node] != 0:
        print '(' + node + ')', "%.2f" % (delay_node[node] / received_node[node],)
    else:
        print '(' + node + ')', -1

print
print "Delivery rates by packet type:"
for t in sent_type.keys():
    if sent_type[t] != 0:
        print t, "%.2f" % (100.0 * received_type[t] / sent_type[t],), '[', received_type[t], '/', sent_type[t], ']', '\t', packetTypes[int(t,16)]
    else:
        print t, -1
print "Fraction of data packets:", "%.2f%%" % (100.0 * received_type['05'] / sum(received_type.values()),)


print
print "Delay by packet type:"
for t in sent_type.keys():
    if received_type[t] != 0:
        print t, "%.2f" % (delay_type[t] / received_type[t],), '\t', packetTypes[int(t,16)]
    else:
        print t, -1

print
print "Overall delivery rate:"
totalSent = sum(sent_type.values())
totalRecv = sum(received_type.values())
print "%.2f" % (100.0 * totalRecv / totalSent,), '[', totalRecv, '/', totalSent, ']'

print
print "Overall delay:"
totalDelay = sum(delay_type.values())
print "%.2f" % (totalDelay / totalRecv,)