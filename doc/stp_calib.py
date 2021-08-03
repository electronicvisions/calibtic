"""
Extraction of STP transformation from S. Billaudelle's  Bachelor-Thesis
"""
import numpy
import pylab

# Extraction trafo for recovery time constant from Figure 19 
# Procedure:
# Linear Fit to the the mean values, with the following data points
# DAC = 5, M = 0.01/us -> T = 1/M = 1e-6/0.01= 1.04e-4 seconds
# DAC = 40, M = 0.065/us -> T = 1/M = 1e-6/0.065 = 
x1 = 5
x2 = 40
y1 = 1./1.04e-4 # according to text, max recovery time is 104 us
y2 = 0.065/1e-6
coeffs_M = numpy.polyfit([x1,x2],[y1,y2], 1)
print(coeffs_M) #[ 1582.41758242 1703.2967033 ]

# Polyfit from y to x (this is the way required for Bio to HW trafo)
# The following coefficients are used
coeffs_reverse = numpy.polyfit([y1,y2], [x1,x2], 1)
print(coeffs_reverse) # [ 6.31944444e-04  -1.07638889e+00]
p1 = numpy.poly1d(coeffs_M)
p2 = numpy.poly1d(coeffs_reverse)

pylab.figure()
pylab.plot(list(range(1023)), p1(list(range(1023))))
pylab.ylabel("M")
pylab.xlabel("V_dtc [DAC]")

pylab.figure()
pylab.plot(list(range(1023)), p1(list(range(1023)))**(-1), label="fit y to x")
pylab.ylabel("T=1/M")
pylab.xlabel("V_dtc [DAC]")

t = numpy.arange(1e-6, 1.04e-4, 1e-6)
m = t**(-1)
pylab.plot(p2(m), t, label="fit x to y")
pylab.legend()
pylab.show()
