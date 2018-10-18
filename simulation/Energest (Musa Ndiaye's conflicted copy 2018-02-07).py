import re,csv,sys
#import panda as pd
import matplotlib.pyplot as plt
# for P lines
#0-> str,
#1 -> clock_time(),2-> P, 3->rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1], 4-> seqno,
#5 -> all_cpu,6-> all_lpm,7-> all_transmit,8-> all_listen,9-> all_idle_transmit,10-> all_idle_listen,
#11->cpu,12-> lpm,13-> transmit,14-> listen, 15 ->idle_transmit, 16 -> idle_listen, [RADIO STATISTICS...]



from collections import defaultdict
cpuOverTime =  defaultdict(list)
f = file(sys.argv[1])
#with open('loglistener4.txt', 'rb') as f:
reader = csv.reader(f,delimiter=' ')

#response = raw_input("Enter ID of the node to be monitored: ")
print("TIME"+"\t      "+"NODE ID"+"\t"+"CPU"+"\t  "+"LPM"+"\t        "+"TX"+"\t  "+"RX")
for row in reader:

	if(len(row) < 3):
		continue
	if row[2] == 'P':
		#print(response)
		for i in range (1,100):
		
			if "ID:"+str(i) in row[0]:
			#row[11]=[int(s) for s in re.split('[\s,]+',row[11])]
			#print(row)
				row[12]=(row[12]+"\t")
			#print(row[12])
				print(row[0]+row[11]+"\t"+row[12]+"\t"+row[13]+"\t"+row[14])
			# plt.plot(row[0].row[11])
			# plt.show()
			
	


	
	#	while(1):
#			a=1
	    	#cpuOverTime[row[3]].append(row[11])

#for i in cpuOverTime:
    #plt.plot(cpuOverTime[i])
#plt.show()
