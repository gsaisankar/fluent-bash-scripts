# -*- coding: utf-8 -*-
"""
Created on Tue Apr  2 02:16:30 2019

@author: sganes3
This piece of code, looks up a Fluent saved results file & extracts data from it and plots the x and y
Last modified for ARPA-E 500L geometry - Mass transfer cases
"""


import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
    
files=[ 50, 100, 200, 400, 600, 800 ]
with open('mean_vof_800') as f:
    fil=f.readlines()[6:]
    fil2=[]
    for l in fil:
        fil2.extend(l.split())
    #print(fil2)
    
#    [words for seg in fil for words in seg.split()]
fil2=fil2[1::2]
x=np.arange(0,3.4,0.2)
plt.plot('x','fil2[0:16]')

#    print(fil)
    #fil=fil.split('           ', 1)
    
#    print(fil)


"""numbers = re.findall(r"[-+]?\d*\.\d+|\d+", file)
print(numbers)

out=numbers[2::3]
inn=numbers[3::3]
intt=10000
#fl_inn=[x / intt for x in inn]
print(out)
print(inn)"""


"""for i in numbers:
    if numbers[i]%3 ==0:
        out[]=numbers[i]
    elif numbers[i]%2 ==0:
        inn[]=numbers[i] 
i=2
out[]
while i<len(numbers):
    out[i]=(numbers[i])
    i=i+3 """


#plt.plot(out, inn)

#import sys
#sys.path.append()
#print(sys.path)