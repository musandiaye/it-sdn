import matplotlib.pyplot as plt
import re
import numpy as np
import pandas as pd
import matplotlib.animation as animation
import math
from matplotlib import style

style.use('classic')
plt.close('all')
#The following are the node specifications (TI wismote Arago CC2520)
iCPU = float(1.6* math.pow(10,-3))#CPU current in Amps
iLPM =  float(175* math.pow(10,-6))#Low Power Mode current in Amps
iTX0 =  float(25.8* math.pow(10,-3))#@ 0dBm transmit power
iTX5 =  float(33.6* math.pow(10,-3))#@ +5dBm transmit power
iRX = float(18.5* math.pow(10,-3))

volt=3#volts
Rtimer_second= 32768
Runtime = 10 # in seconds

fig = plt.figure()
ax = fig.add_subplot(1,1,1)

#def animate (i) :
#------------------------------------------------------------------
graph_data = open ('energestdata.txt', 'rt').read()#to compare with another dataset repeat the code and add 2 to all the variable ite

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
bdf1=pd.concat([rtdf,df], axis=1) #concatenate two data arrays
	#print bdf
	
#ploting from a dataframe 
#---------------------------------------------------------------------------------------------------------------
E1 = np.add(bdf1['Ecpu'],bdf1['Elpm'])
E2 = np.add(bdf1['Etx'],bdf1['Erx'])
Etotal1 = np.add(E1,E2)
#t=bdf['Run']
#---------------------------------------------------------------------------------------------------------------------
graph_data = open ('Id_data.txt', 'rt').read()#to compare with another dataset repeat the code and add 2 to all the variable ite

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
bdf2=pd.concat([rtdf,df], axis=1) #concatenate two data arrays
	#print bdf
	
#ploting from a dataframe   
#---------------------------------------------------------------------------------------------------------------
E1 = np.add(bdf2['Ecpu'],bdf2['Elpm'])
E2 = np.add(bdf2['Etx'],bdf2['Erx'])
Etotal2 = np.add(E1,E2)
#t=bdf['Run']
#-------------------------------------------------------------------------------
  
ax.plot(bdf1['Run'],Etotal1,label='Total')
ax.plot(bdf2['Run'],Etotal2,'-x',label='Id')
ax.set_title('Power Consumption')
ax.set_ylim([-0.01,0.4])
ax.set_xlabel('Time(s)')
ax.set_ylabel('Power (mW)')
ax.legend()

#ani = animation.FuncAnimation (fig,animate, interval=100000)

plt.show()