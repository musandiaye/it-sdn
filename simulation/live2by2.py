import matplotlib.pyplot as plt
import re
import numpy as np
import pandas as pd
import matplotlib.animation as animation
import math
from matplotlib import style

style.use('seaborn-paper')
plt.close('all')
#x = np.linspace(0, 2 * np.pi, 400)
#y = np.sin(x *2)
#The following are the node specifications (TI wismote Arago CC2520)
iCPU = float(1.6* math.pow(10,-3))#CPU current in Amps
iLPM =  float(175* math.pow(10,-6))#Low Power Mode current in Amps
iTX0 =  float(25.8* math.pow(10,-3))#@ 0dBm transmit power
iTX5 =  float(33.6* math.pow(10,-3))#@ +5dBm transmit power
iRX = float(18.5* math.pow(10,-3))
volt=3#volts
Rtimer_second= 12728
Runtime = 10 # in seconds

fig = plt.figure()
fig2 = plt.figure()
ax1 = fig.add_subplot(2,2,1)
ax2 = fig.add_subplot(2,2,2)
ax3 = fig.add_subplot(2,2,3)
ax4 = fig.add_subplot(2,2,4)
ax = fig2.add_subplot(1,1,1)

#l = []
#f, axarr = plt.subplots(2, 2)
#dec=re.compile(r"\bd\w*r\b")
def animate (i) :
	
	graph_data = open ('Id_data.txt', 'rt').read()
	
	lines = graph_data.split('\n')
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
	
	#resolves the runtime for the x plot
	runtimet= int(len(clock)*Runtime)+10
	run=np.arange(10, runtimet,10)
	
	#clock = [() for x in clock]#This is a way to add a fomular to a list in this case (x *2) where x is the variable in the fomular
	#Ecpu = [ePower(x,iCPU) for x in Ecpu]
	Ecpu = [((x * volt * iCPU)/(Rtimer_second*Runtime)) for x in Ecpu]
	Elpm = [((x * volt * iLPM)/(Rtimer_second*Runtime)) for x in Elpm]
	Etx = [((x * volt * iTX0)/(Rtimer_second*Runtime)) for x in Etx]
	Erx = [((x * volt * iRX)/(Rtimer_second*Runtime)) for x in Erx]
	E1 = np.add(Ecpu,Elpm)
	E2 = np.add(Etx,Erx)
	Etotal = np.add(E1,E2)
	ax1.plot(run,Ecpu)#plot (x,y)
	#ax1.set_ylim([0.00001,0.001])
	ax1.set_title('CPU')
	ax1.set_xlabel('Time (s)')
	ax1.set_ylabel('Power(W)')

	ax2.plot(run,Elpm)
	ax2.set_title('LPM')
	ax2.set_xlabel('Time(s)')
	ax2.set_ylabel('Power(W)')

	ax3.plot(run, Etx)
	ax3.set_title('Transmit')
	ax3.set_xlabel('Time(s)')
	ax3.set_ylabel('Power(W)')

	ax4.plot(run, Elpm)
	ax4.set_title('Receive')
	ax4.set_xlabel('Time(s)')
	ax4.set_ylabel('Power(W)')
	
	ax.plot(run,Etotal)
	ax.set_title('Total Power Consumption')
	ax.set_xlabel('Time(s)')
	ax.set_ylabel('Power (W)')
	

# Fine-tune figure; hide x ticks for top plots and y ticks for right plots
#plt.setp([a.get_xticklabels() for a in ax1[0, :]], visible=False)
#plt.setp([a.get_yticklabels() for a in axarr[:, 1]], visible=False)
	
	
	
ani = animation.FuncAnimation (fig,animate , interval=1000)
ani2= animation.FuncAnimation (fig2,animate , interval=1000)
plt.show()