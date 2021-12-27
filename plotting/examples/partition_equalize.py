# -*- coding: utf-8 -*-
"""
Example of how partition
that is range-of-nth nth_element 
can be used to equalize data

@author: johan
"""


import numpy
import numpy as np
from PIL import Image
nan=np.nan
from matplotlib import pyplot as plt 
import matplotlib

def savePhotoSmaller(img,basename):
    basewidth=min(1500,img.size[0])
    wpercent = (basewidth/float(img.size[0]))
    hsize = int((float(img.size[1])*float(wpercent)))    
    img = img.resize((basewidth,hsize), Image.ANTIALIAS)
    img.save("out_small/"+basename+'.small.png',optimize = True)
    img.save("out_small/"+basename+'.small.jpg',quality=45,optimize = True)

def savePhoto(img,basename):
    img.save("out_large/"+basename+".png",optimize=True)
    img.save("out_large/"+basename+".jpg",quality=20,optimize= True)
    savePhotoSmaller(img,basename)

def saveFig(fg,name):
    fg.savefig("out_large/"+name, dpi=600,bbox_inches='tight')

for imgname in ("einstein","forsen"):
    
    im = Image.open(imgname+".png")
    data = np.asarray(im)
    ColMax=2**16-1.0 # values in image
    dispScale=2**8 # needed a scale to round-trip, probably some format
                   # option I didn't know about
    data = np.asarray(im)
    x=data.shape[0]
    y=data.shape[1]
    N=x*y
    
    
    #roundtrip check with same image sizes and compression
    if True:
        roundtrip = Image.fromarray(data/dispScale)
        roundtrip = roundtrip.convert('RGB')
        savePhoto(roundtrip,imgname+"_roundtrip")
    
    # equalize using sort
    flat=data.reshape([1,N])[0]
    flat_sorted= np.sort(flat)
    flat_new_range=np.arange(0,N,1)/((N-1)/ColMax)        
    equalized=np.interp(flat,flat_sorted,flat_new_range)
    equalized_xy = np.reshape(equalized,[x,y])
    img_normalize = Image.fromarray(equalized_xy/dispScale)
    img_normalize = img_normalize.convert('RGB')
    savePhoto(img_normalize,imgname+"_sort")
    
    
    fig, ax1 = plt.subplots() 
    fig.set_figwidth(4)
    fig.set_figheight(4)
    ax1.grid(color='#aaaaaa', linestyle='-', linewidth=0.37)
    color="#666666"
    line=ax1.plot(flat_new_range/ColMax,flat_sorted/ColMax,'-',color=color,linewidth=0.7,label="sort")
    
    fig.show() 
    
    # approximate equalize using 2,3 or 5 partition points to
    # numpy.partition 
    
    colors=['red','green','blue','purple','#bbbb00',"#888888","#ff9999","#66ff66","#6666ff","#ff66ff","black","#006600","#660000","#000066","#660066"]
    color_i=0
    for m in  [7,5,3,2]:
        if 'forsen' == imgname and m == 7:
            continue
        nths=[int(i) for i in numpy.arange(0,N,(N-1)/(m-1-11e-8))]
        if not nths[-1] == N-1:
            nths.append(N-1)
        print(m)
        print(list(round(i/(N-1),6) for i in nths))
        color=colors[color_i]
        color_i=color_i+1
    
        flat_part= np.partition(flat,nths)
        flat_cumulants=numpy.array([flat_part[i] for i in nths])
        flat_new_range_at_cumulants=np.arange(0,m,1)/((m-1)/ColMax)        
        equalized_nths=np.interp(flat,flat_cumulants,flat_new_range_at_cumulants)
        equalized_xy_nths = np.reshape(equalized_nths,[x,y])
        img_normalize_nths = Image.fromarray(equalized_xy_nths/dispScale)
        img_normalize_nths = img_normalize_nths.convert('RGB')
        #img_normalize_nths.save(imgname+"_partition"+str(m)+".png")
        savePhoto(img_normalize_nths,imgname+"_partition"+str(m))
    
        line=ax1.plot(flat_new_range_at_cumulants/ColMax,flat_cumulants/ColMax,'*-',markersize=3.4, color=color,linewidth=0.4+0.15*m,label="m={}".format(m))
    
    
    ax1.set_xlim([0,1])
    ax1.legend(loc=2, prop={'size': 6})
    ax1.yaxis.get_ticklocs(minor=True)
    ax1.minorticks_on()
    ax1.grid(visible=True, which='minor', color='gray', linestyle='-',linewidth=0.2, alpha=0.2)
    #ax1.set_title("sort",fontsize=4)
    
    ax1.set_xticks(numpy.arange(0,1,0.1-1e-8))
    ax1.set_yticks(numpy.arange(0,1,0.1-1e-8))
    ax1.set_xlabel("index / (N-1)",fontsize=8) 
    ax1.set_ylabel('pixel value / (2^16-1)',fontsize=8)
    ax1.set_title(imgname,fontsize=8)
    fig.show() 
    
    
    saveFig(fig,imgname+"_cdf_approximation.png")
    
