#control traffic ratio, packet delivery ratio, use regular expressions 
#use regular expressions for this kind of data input 
# remove all digits using \D expressions in re.compile
import numpy as np
import csv,re
import pandas as pd
import matplotlib.pyplot as plt 
from matplotlib import style
import math 



style.use('seaborn-poster')
plt.close('all')
fig = plt.figure()
fig2 = plt.figure()
ax1 = fig.add_subplot(2,1,1)
ax2 = fig.add_subplot(2,1,2)
ax = fig2.add_subplot(1,1,1)

Hour=[]
Minute=[]
Second=[]
time=[]
A=[]
B=[]
C=[]
D=[]
control_data= open('stat.txt').read()
found_delivery_rates = 0
found_delay = 0
found_time = 0
lines = control_data.split('\n')
saved_line = ''
for line in lines:
		
	#num = re.sub(r'\D', "", line)# insert a regex to filter in time stamp,fraction of data packets, and overall delivery rates
	#use regex to print everything preceded by a :
	#if len(line) > 1 :
	
	if(found_time == 1):
		Hour=[]
		Minute=[]
		Second=[]
		print(saved_line)
		saved_linenum=re.sub(r'\D'," ",saved_line)
		print saved_linenum
		H,M,S=saved_linenum.split()
		Hour.append(H)
		Minute.append(M)
		Second.append(S)
		
		Hour=map(int,Hour)
		Minute=map(int,Minute)
		Second=map(int,Second)
		print Hour
		print Minute
		
		Hsec=[(x*3600) for x in Hour]
		Msec=[(x*60) for x in Minute]
		time= Hsec+Msec+Second
		time=sum(time)
		print time
		A.append(time)
		found_time = 0
	
	if line.find("Delivery rates by source node:")!=-1:
			found_time = True
			continue
	if(line != ""):
		saved_line = line	
		
		
	if line.find("Fraction of data packets: ")!=-1:
			print line#remove non digit values and append to a column
			line_r=re.sub(r'[^\d,.]',"",saved_line)#using regular expressions to filter out the required data
			B.append(line_r)
			B=map(float,B)
	if line.find("Overall delivery rate")!=-1:
			found_delivery_rates = True
			continue
	if(found_delivery_rates == 1):
		print"Overall delivery rate is ",line
		line_r=re.sub(r'\[(.*?)\]',"",line)#(.*?) : match everything in a non-greedy way and capture it, the match is then replaced with a blank
		C.append(line_r)
		C=map(float,C)
		found_delivery_rates = 0		
			
			
	if line.find("Overall delay")!=-1:
		found_delay = True
		continue#this continue makes it go to the next line		
	if(found_delay == 1):
		print "Overall delay is ",line
		D.append(line)
		D=map(float,D)
		found_delay = 0
for i in range(1,len(A)):#creates a time axis in seconds
	A[i] = A[i] - A[0]
A[0] = A[0]-A[0]

for i in range(0,len(B)):# converts the fraction of data to fraction of control traffic
	B[i] = (100.0 - B[i])/100
	
for i in range(0,len(C)):# converts to a ratio
	C[i] = C[i]/100

#The following creates columns for the parameters
d={'Time(s)':A,'controlTraf_ratio':B,'Delivery_ratio':C,'Delay(ms)':D}
df=pd.DataFrame(d)
print df


#plot the available graphs
ax1.plot(df['Time(s)'],df['controlTraf_ratio'],'-')
ax1.set_title('Control Traffic Overhead')
#ax1.set_ylim([-0.01,1])
ax1.set_xlabel('Time(s)')
ax1.set_ylabel('Ratio')

ax2.plot(df['Time(s)'],df['Delivery_ratio'],'o')
ax2.set_title('Packet Delivery Ratio (PDR)')
#ax2.set_ylim([-0.01,1])
ax2.set_xlabel('Time(s)')
ax2.set_ylabel('Ratio')

ax.plot(df['Time(s)'],df['Delay(ms)'],'o')
ax.set_title('Average Packet Delay')
ax.set_ylim([0,220])

ax.set_xlabel('Time(s)')
ax.set_ylabel('Delay (ms)')



plt.show()