# -*- coding: utf-8 -*-
"""
Created on Tue May 11 20:59:58 2021
@author: johan
"""

import numpy
import time
import numpy as np 
import json
import math
from matplotlib import pyplot as plt 
import matplotlib
inf=numpy.nan

random_partitions=False
if random_partitions:
    casename = "partition_points_random"
    fname= "timing_data/datatableLarger_n1.txt"
    label="random partition points"

    casename = "mqs_med3_rnd"
    fname = "timing_data/dataLarger_"+casename+".txt"

else:
    casename = "partition_points_equidistant"
    fname= "timing_data/datatableLarger_n1_equi.txt"
    label="equidistant partition points"

    casename = "ninther_mintroselect_equi"
    fname = "timing_data/dataLarger_"+casename+".txt"

    label="equidistant partition points intro ninther"

datafile="\n".join([l for l in open(fname).readlines() if not "#" in l]).replace("inf","10000.0")
d=json.loads(datafile)
data=np.array(d)

oRounds=sorted({k[0] for k in data})
Ns=sorted({k[1] for k in data})

def removeOutliers(data,outliers):
    if len(data)-2*outliers<2:
        return data
        #raise Exception("too little data")
    return numpy.partition(data,[outliers,len(data)-outliers-1],axis=0)[outliers:len(data)-outliers-1]
#    return numpy.partition(data,[outliers,trials-outliers-1],axis=0)[outliers:trials-outliers-1]

def saveFig(fg,name):
    fg.savefig(name, dpi=600,bbox_inches='tight')
    

fig, ax1 = plt.subplots() 
fig.set_figwidth(4)
fig.set_figheight(4)
ax1.grid(color='#aaaaaa', linestyle='-', linewidth=0.37)

summary=""

colors=['gray','green','blue','black','purple','red','magenta']
colors_i=0
for i,N in enumerate(Ns):
    ys=[]
    x=[]
    msActual=[]
    if N < 150:
        continue
    color=colors[colors_i]
    colors_i=colors_i+1

    for round_i in oRounds:
        m=sorted([[k[3],k[4],k[3]] for k in data if k[0]==round_i and k[1] == N])
        try:
            x,y,mActual = list(zip(*m))
            x=np.array(x)
            ys.append(y)
            msActual.append(mActual)
        except:
            print("undexpected data shape")

    msActualNo = numpy.array(msActual)
    msActualMean=numpy.mean(msActual,axis=0)
    #msActualStd=numpy.std(msActual,axis=0)
    x=msActualMean

    ax1.set_ylabel("speed factor vs std::sort") 
    ax1.set_xlabel("number of partition points") 
    ax1.set_xscale('log')
    #ax1.set_yscale('log')

    ys=numpy.array(ys)
    outliers=3
    trials= ys.shape[0]
    ysStd=numpy.std(ys,axis=0)
    ys=removeOutliers(ys,outliers)
    #ys=numpy.partition(ys,[outliers,trials-outliers-1],axis=0)[outliers:trials-outliers-1]
   
    ysMean=numpy.mean(ys,axis=0)
    line=plt.errorbar(x,ysMean,ysStd, fmt='.-',ecolor=color,color=color,linewidth=0.5)
    #line=plt.errorbar(x,ysMean,ysStd, msActualStd, fmt='.-',ecolor=colors[i],color=colors[i])
    line.set_label(("N="+"%1.0e"%int(N)).replace("+0",""))
    #ref=len(ysMean)-3
    import bisect
    #ref2=  bisect.bisect(x,N/100.0)
    ref=len(ysMean)-4
    #ref=min(ref,ref2)
    thinLine=numpy.divide(ysMean[ref]*numpy.log(x[ref]),numpy.log(x))
    for x_i in range(len(x)):
        if x[x_i]<20 or thinLine[x_i]<0.6:
            thinLine[x_i]=math.nan
    ax1.plot(x,thinLine,':',color=color,linewidth=0.2)
    if(N>300):
        summary=summary+("\nN: %.0e"%N).replace("+0","")
        summary+="\n  m: "+" ".join(["%6.0f"%f for f in x])
        summary+="\n su: "+" ".join(["%6.1f"%f for f in ysMean])

print(summary)

ax1.set_xlim([0.9, 120000*10])
#ax1.set_xlim([0.9, 12000])
#ax1.set_xlim([0.9, 1200])
ax1.set_yticks(range(0,12))
ax1.set_xticks(numpy.power(10.0,numpy.arange(-1,10,1)))
ax1.set_xticks(numpy.power(10.0,numpy.arange(-1,10,0.1)),minor=True)
ax1.set_ylim([0., 12])
ax1.set_xlim([0.9, 120000*10])
ax1.legend(loc=1, prop={'size': 6})
ax1.yaxis.get_ticklocs(minor=True)
ax1.xaxis.get_ticklocs(minor=True)
ax1.minorticks_on()
y_minor = matplotlib.ticker.LogLocator(base = 10.0, subs = numpy.arange(1.0, 10.0) , numticks = 10)
ax1.xaxis.set_minor_locator(y_minor)

ax1.grid(visible=True, which='minor', color='gray', linestyle='-',linewidth=0.2, alpha=0.2)
ax1.set_title(label,fontsize=4)
#ax1.set_xlim([20, 1200000])
#ax1.set_ylim([0.65, 3])

#plt.errorbar(x,y,y*0.1, fmt='k.-')

fig.show() 
saveFig(fig,"images/"+casename+"_speed_for_m_vs_sort"+".png")


fig, ax1 = plt.subplots() 
fig.set_figwidth(4)
fig.set_figheight(4)
ax1.grid(color='#aaaaaa', linestyle='-', linewidth=0.37)


Ms=sorted({k[2] for k in data})
xMinShow=0

colors=['red','green','blue','#888800','purple','#bbbb00',"#888888","#ff9999","#66ff66","#6666ff","#ff66ff","black","#006600","#660000","#000066","#660066"]
color_i=0
for i,M in enumerate(Ms):
    ys=[]
    x=[]    
    msActual=[]
    for round_i in oRounds:
        theseNs=sorted([[k[1],k[4],k[3]] for k in data if k[0]==round_i and k[2] == M])
        x,y,mActual = list(zip(*theseNs))
        x=np.array(x)
        ys.append(y)
        msActual.append(mActual)
    if len(x)<2:
        continue

    msActualNp=numpy.array(msActual)
    msActualMean=numpy.mean(msActualNp,axis=0)
    
    color=colors[color_i]
    color_i=color_i+1

    ax1.set_ylabel("speed factor vs std::sort") 
    ax1.set_xlabel("N") 
    ax1.set_xscale('log')
    #ax1.set_yscale('log')
    
    outliers=3
    ys=numpy.array(ys)
    trials= ys.shape[0]
    ysStd=numpy.std(ys,axis=0)
    ys=removeOutliers(ys,outliers)
    #ys=numpy.partition(ys,[outliers,trials-outliers-1],axis=0)[outliers:trials-outliers-1]
    ysMean=numpy.mean(ys,axis=0)
    #ysStd=numpy.std(ys,axis=0)
    for j in range(len(ysMean)):
        if x[j]<M*4 or x[j]< xMinShow:
            ysMean[j]=math.nan
            x[j]=math.nan

    line=plt.errorbar(x,ysMean,ysStd, fmt='.-',ecolor=colors[i],color=color,linewidth=0.5)
    line.set_label("m="+str(int(M)))
    ref=len(ysMean)-2
    ax1.plot(x,numpy.multiply(ysMean[ref]/numpy.log(x[ref]),numpy.log(x)),':',color=color,linewidth=0.3)
    

ax1.set_xlim([200, 3500000*10])
#ax1.set_xlim([0.9, 12000])
#ax1.set_xlim([0.9, 1200])
ax1.set_yticks(range(0,12))
ax1.set_ylim([0.65, 12])
ax1.legend(loc=2, prop={'size': 6})
ax1.yaxis.get_ticklocs(minor=True)
ax1.minorticks_on()
ax1.grid(visible=True, which='minor', color='gray', linestyle='-',linewidth=0.2, alpha=0.2)
y_minor = matplotlib.ticker.LogLocator(base = 10.0, subs = numpy.arange(1.0, 10.0) , numticks = 10)
ax1.xaxis.set_minor_locator(y_minor)
ax1.set_title(label,fontsize=4)

fig.show() 
saveFig(fig,"images/"+casename+"_speed_for_n_vs_sort"+".png")


fig, ax1 = plt.subplots() 
fig.set_figwidth(4)
fig.set_figheight(4)
ax1.grid(color='#aaaaaa', linestyle='-', linewidth=0.37)


Ms=sorted({k[2] for k in data})
xMinShow=0

colors=['red','green','blue','#888800','purple','#bbbb00',"#888888","#ff9999","#66ff66","#6666ff","#ff66ff","black","#006600","#660000","#000066","#660066"]
color_i=0
for i,M in enumerate(Ms):
    ys=[]
    ysS=[]
    x=[]    
    msActual=[]
    for round_i in oRounds:
        theseNs=sorted([[k[1],k[4]*k[6],k[3],k[6]] for k in data if k[0]==round_i and k[2] == M])
        x,y,mActual,Tnth = list(zip(*theseNs))
        x=np.array(x)
        ys.append(Tnth)
        ysS.append(y)
        msActual.append(mActual)
    if len(x)<2:
        continue

    msActualNp=numpy.array(msActual)
    msActualMean=numpy.mean(msActualNp,axis=0)
    

    ax1.set_ylabel("time") 
    ax1.set_xlabel("N") 
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    #ax1.set_yscale('log')


    color=colors[color_i]
    color_i=color_i+1
    

    outliers=3
    ys=numpy.array(ys)
    trials= ys.shape[0]
    ysStd=numpy.std(ys,axis=0)
    #ys=numpy.partition(ys,[outliers,trials-outliers-1],axis=0)[outliers:trials-outliers-1]
    ysMean=numpy.mean(ys,axis=0)
    #ysStd=numpy.std(ys,axis=0)
    #for j in range(len(ysMean)):
    #    if x[j]<M*4 or x[j]< xMinShow:
    ##        ysMean[j]=math.nan
    #        x[j]=math.nan

    line=ax1.errorbar(x,ysMean,ysStd, fmt='.-',ecolor=color,color=color,markersize=1.5,linewidth=0.5)
    line.set_label("m="+str(int(M)))
    ref=len(ysMean)-2
    def shape(x):
        return x
    ax1.plot(x,numpy.multiply(ysMean[ref]/shape(x[ref]),shape(x)),':',color=color,linewidth=0.3)

    ysOrg=ys
    if M == 1000:
        outliers=3
        ys=ysS
        ys=numpy.array(ys)
        trials= ys.shape[0]
        ysStd=numpy.std(ys,axis=0)
        #ys=numpy.partition(ys,[outliers,trials-outliers-1],axis=0)[outliers:trials-outliers-1]
        ysMean=numpy.mean(ys,axis=0)
        #ysStd=numpy.std(ys,axis=0)
        #for j in range(len(ysMean)):
        #    if x[j]<M*4 or x[j]< xMinShow:
        #        ysMean[j]=math.nan
        #        x[j]=math.nan

        color="#aaaaaa"    
        # trick to work around plotting issue, just draw first point first:
        line=ax1.errorbar(x[0],ysMean[0],ysStd[0], fmt='.-',ecolor=color,color=color,markersize=2.5,linewidth=0.5)
        line.set_label("std::sort")
        # then error bar without line:
        line=ax1.errorbar(x,ysMean,ysStd, fmt='.',ecolor=color,color=color,markersize=2.5,linewidth=0.5)
        # then line:
        ax1.plot(x,ysMean,ysStd,'-',color=color,linewidth=0.6)
        ref=len(ysMean)-2
        def shape(x):
            return x*numpy.log(x)
        color="#444444"    
        ax1.plot(x,numpy.multiply(ysMean[ref]/shape(x[ref]),shape(x)),':',color=color,linewidth=0.3)
    ys=ysOrg



ax1.set_xlim([200, 3500000*10])
#ax1.set_xlim([0.9, 12000])
#ax1.set_xlim([0.9, 1200])
#ax1.set_yticks(range(0,12))
ax1.set_ylim([1e-6,6])
handles, labels = ax1.get_legend_handles_labels()
ax1.legend(handles[::-1], labels[::-1],loc=2, prop={'size': 6})
ax1.yaxis.get_ticklocs(minor=True)
ax1.minorticks_on()
ax1.grid(visible=True, which='minor', color='gray', linestyle='-',linewidth=0.2, alpha=0.2)
ax1.set_title(label,fontsize=4)

fig.show() 
saveFig(fig,"images/"+casename+"_time_for_n"+".png")

#############################################################
#
#
#############################################################

def nlogn(x):
    return x*numpy.log(x)

def scaledownFunc(x):
    return nlogn(x)


for fractional in [False, True]:
    fig, ax1 = plt.subplots() 
    fig.set_figwidth(4)
    fig.set_figheight(4)
    ax1.grid(color='#aaaaaa', linestyle='-', linewidth=0.37)
    
    colors=['red','green','blue','#888800','purple','#bbbb00',"#888888","#ff9999","#66ff66","#6666ff","#ff66ff","black","#006600","#660000","#000066","#660066"]
    color_i=0
    refScale=0
    Ms=sorted({k[2] for k in data})
    xMinShow=0
    if fractional:
        xScale=lambda v: v/M
    else:
        xScale=lambda v: v
        
    for i,M in enumerate(Ms):
        ys=[]
        ysS=[]
        x=[]    
        msActual=[]
        for round_i in oRounds:
            theseNs=sorted([[k[1],k[4]*k[6],k[3],k[6]] for k in data if k[0]==round_i and k[2] == M])
            x,y,mActual,Tnth = list(zip(*theseNs))
            x=np.array(x)
            ys.append(Tnth/scaledownFunc(x))
            ysS.append(y/scaledownFunc(x))
            msActual.append(mActual)
        if len(x)<2:
            continue
    
        msActualNp=numpy.array(msActual)
        msActualMean=numpy.mean(msActualNp,axis=0)
        
    
        ax1.set_ylabel("time / N log N. Scaled to std::sort at N=3e6") 
        if fractional:
            ax1.set_xlabel("N/m") 
        else:
            ax1.set_xlabel("N") 
        #ax1.set_yscale('log')
    
    
        ysOrg=ys
        if i == 0 :
            outliers=3
            ys=ysS
            ys=numpy.array(ys)
            trials= ys.shape[0]
            ysStd=numpy.std(ys,axis=0)
            ysMean=numpy.mean(ys,axis=0)
    
            ref=len(ysMean)-2
            refScale=1/ysMean[ref]
    
            color="#aaaaaa"    
            if not fractional:
                # trick to work around plotting issue, just draw first point first:
                line=ax1.errorbar(xScale(x[0]),refScale*ysMean[0],refScale*ysStd[0], fmt='.-',ecolor=color,color=color,markersize=2.5,linewidth=0.5)
                line.set_label("std::sort")
                # then error bar without line:
                line=ax1.errorbar(xScale(x),refScale*ysMean,refScale*ysStd, fmt='.',ecolor=color,color=color,markersize=2.5,linewidth=0.5)
                ax1.plot(xScale(x),refScale*ysMean,refScale*ysStd,'-',color=color,linewidth=0.6)
                def shape(x):
                    return nlogn(x)/scaledownFunc(x)
                color="#444444"    
                ax1.plot(xScale(x),refScale*numpy.multiply(ysMean[ref]/shape(x[ref]),shape(x)),':',color=color,linewidth=0.3)
    
        ys=ysOrg
    
        color=colors[color_i]
        color_i=color_i+1
        
    
        outliers=3
        ys=numpy.array(ys)
        trials= ys.shape[0]
        ysStd=numpy.std(ys,axis=0)
        #ys=numpy.partition(ys,[outliers,trials-outliers-1],axis=0)[outliers:trials-outliers-1]
        ysMean=numpy.mean(ys,axis=0)
        #ysStd=numpy.std(ys,axis=0)
        #for j in range(len(ysMean)):
        #    if x[j]<M*4 or x[j]< xMinShow:
        ##        ysMean[j]=math.nan
        #        x[j]=math.nan
    
                
        line=ax1.errorbar(xScale(x),refScale*ysMean,refScale*ysStd, fmt='.-',ecolor=color,color=color,markersize=1.5,linewidth=0.5)
        line.set_label("m="+str(int(M)))
        ref=len(ysMean)-2
        def shape(x):
            return x/scaledownFunc(x)
    
        thinLine=refScale*numpy.multiply(ysMean[ref]/shape(x[ref]),shape(x))
        xMinShow = 5000
        for j in range(len(ysMean)):
            if x[j]< xMinShow:
                thinLine[j]=math.nan
    
        ax1.plot(xScale(x),thinLine,':',color=color,linewidth=0.3)
    
    
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    ax1.set_yscale('linear')
    
    if fractional:
        ax1.set_xlim([200,1e8])
        ax1.set_ylim([1e-6,1])
    else:
        ax1.set_xlim([200, 3500000*10*10])
        ax1.set_ylim([1e-6,1.2])
    
    #ax1.set_xlim([0.9, 12000])
    #ax1.set_xlim([0.9, 1200])
    #ax1.set_yticks(range(0,12))
    handles, labels = ax1.get_legend_handles_labels()
    handles, labels = ax1.get_legend_handles_labels()
    ax1.legend(handles[::-1], labels[::-1],loc=1, prop={'size': 4})
    ax1.yaxis.get_ticklocs(minor=True)
    ax1.minorticks_on()
    ax1.grid(visible=True, which='minor', color='gray', linestyle='-',linewidth=0.2, alpha=0.2)
    ax1.set_title(label,fontsize=4)
    fig.show() 
    suffix="_frac" if fractional else "_div"
    saveFig(fig,"images/"+casename+"_time_for_n"+suffix+".png")




