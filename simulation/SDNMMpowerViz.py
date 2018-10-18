import matplotlib.pyplot as plt
import re,csv,sys
import numpy as np
import pandas as pd
import matplotlib.animation as animation
import math
from matplotlib import style

style.use('seaborn-paper')
plt.close('all')
#The following are the node specifications (skymote)
iCPU = float(1.8* math.pow(10,-3))#CPU current in Amps
iLPM =  float(51* math.pow(10,-6))#Low Power Mode current in Amps
iTX0 =  float(19.5* math.pow(10,-3))#@ 0dBm transmit power
#iTX5 =  float(33.6* math.pow(10,-3))#@ +5dBm transmit power
iRX = float(21.8* math.pow(10,-3))

volt=3#volts
Rtimer_second= 32768
Runtime = 10 # in seconds

fig = plt.figure()
fig2 = plt.figure()
ax1 = fig.add_subplot(2,2,1)
ax2 = fig.add_subplot(2,2,2)
ax3 = fig.add_subplot(2,2,3)
ax4 = fig.add_subplot(2,2,4)
ax = fig2.add_subplot(1,1,1)


def animate (i) :
	
	graph_data = open (sys.argv[1], 'rt').read()
	lines = graph_data.split('\n')
	#print type(lines)
	#lines = reader
	time = []#Time stamp from the energest data
	Id = [] #Node Id
	clock = [] # time data 
	Ecpu = []#energest values 
	Elpm = []
	Etx = []
	Erx = []
	#for line in lines[1: ]:#This means start from the second row to the last 
	
	for line in lines:
		
		num = re.sub(r'\D', "", line)#This deletes every non digit items
		#print num
		if len(num) > 1 :
			t, i , c, ec,el,et,er = line.split()#This creates columns for the parameters
			time.append(t)
			Id.append(i)
			clock.append(c)
			Ecpu.append(ec)
			Elpm.append(el)
			Etx.append(et)
			Erx.append(er)
	clock = map (int, clock)#This converts the list items to int from string
	Ecpu = map (int, Ecpu)
	Elpm = map (int, Elpm)
	Etx = map (int, Etx)
	Erx = map (int, Erx)
	
	
	#clock = [(x*2) for x in clock]#This is a way to add a fomular to a list in this case (x *2) where x is the variable in the fomular
	#Ecpu = [ePower(x,iCPU) for x in Ecpu]multiply by 1000 to present in uW checks docs for units
	Ecpu = [((x * volt * iCPU)/(Rtimer_second*Runtime)) for x in Ecpu]#remove runtime to have mJ 
	Elpm = [((x * volt * iLPM)/(Rtimer_second*Runtime)) for x in Elpm]
	Etx = [((x * volt * iTX0)/(Rtimer_second*Runtime)) for x in Etx]
	Erx = [((x * volt * iRX)/(Rtimer_second*Runtime)) for x in Erx]
	
	#The following lines convert the data into a five column dataframe
	d = {'Clock':clock, 'Ecpu':Ecpu , 'Elpm':Elpm , 'Etx':Etx, 'Erx':Erx}#Converts to dict
	df=pd.DataFrame(d)
	#print df
	df= df.groupby(['Clock'])['Ecpu','Elpm','Etx','Erx'].apply(sum).reset_index()
	#print df
	runtimet= int(len(df['Clock'])*Runtime)+10
	run=np.arange(10, runtimet,10)
	rt={'Run':run}
	rtdf=pd.DataFrame(rt)
	bdf=pd.concat([rtdf,df], axis=1) #concatenate two data arrays
	#print bdf
	
	#ploting from a dataframe 
	#---------------------------------------------------------------------------------------------------------------
	E1 = np.add(bdf['Ecpu'],bdf['Elpm'])
	E2 = np.add(bdf['Etx'],bdf['Erx'])
	Etotal = np.add(E1,E2)
	ax1.plot(bdf['Run'],bdf['Ecpu'])#plot (x,y)
	#ax1.set_ylim([0.00001,0.001])  
	ax1.set_title('CPU')
	ax1.set_xlabel('Time (s)')
	ax1.set_ylabel('Power(mW)')

	ax2.plot(bdf['Run'],bdf['Elpm'],label='elpm')
	#ax2.plot(run,Etotal,label='etotal')
	ax2.set_title('LPM')
	ax2.set_xlabel('Time(s)')
	ax2.set_ylabel('Power(mW)')
	#ax.legend()

	ax3.plot(bdf['Run'],bdf['Etx'])
	ax3.set_title('Transmit')
	ax3.set_xlabel('Time(s)')
	ax3.set_ylabel('Power(mW)')

	ax4.plot(bdf['Run'],bdf['Erx'])
	ax4.set_title('Receive')
	ax4.set_xlabel('Time(s)')
	ax4.set_ylabel('Power(mW)')   
	
	ax.plot(bdf['Run'],Etotal)
	ax.set_title('Total Power Consumption')
	#x.set_ylim([0.13,0.14])
	ax.set_xlabel('Time(s)')
	ax.set_ylabel('Power (mW)')
	

# Fine-tune figure; hide x ticks for top plots and y ticks for right plots
#plt.setp([a.get_xticklabels() for a in ax1[0, :]], visible=False)
#plt.setp([a.get_yticklabels() for a in axarr[:, 1]], visible=False)

	
	
ani = animation.FuncAnimation (fig,animate , interval=100000)
ani2= animation.FuncAnimation (fig2,animate , interval=100000)
plt.show()
