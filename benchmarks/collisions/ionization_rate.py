execfile("../../scripts/Diagnostics.py")

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

# Ionization cross section (code units) for Al3+ vs energy (keV)
cs=np.array([[0.001000,0.000000], [0.001177,0.000000], [0.001385,0.000000],
	[0.001630,0.000000], [0.001918,0.000000], [0.002257,0.000000], [0.002656,0.000000],
	[0.003126,0.000000], [0.003678,0.000000], [0.004329,0.000000], [0.005094,0.000000],
	[0.005995,0.000000], [0.007055,0.000000], [0.008302,0.000000], [0.009770,0.000000],
	[0.011498,0.000000], [0.013531,0.000000], [0.015923,0.000000], [0.018738,0.000000],
	[0.022051,0.000000], [0.025950,0.000000], [0.030539,0.000000], [0.035938,0.000000],
	[0.042293,0.000000], [0.049770,0.000000], [0.058570,0.000000], [0.068926,0.000000],
	[0.081113,0.000000], [0.095455,0.000000], [0.112333,0.000000], [0.132194,0.008498],
	[0.155568,0.024568], [0.183074,0.042322], [0.215444,0.059764], [0.253537,0.074867],
	[0.298365,0.087020], [0.351120,0.096034], [0.413202,0.101988], [0.486261,0.105125],
	[0.572238,0.105786], [0.673416,0.104358], [0.792484,0.101245], [0.932605,0.096836],
	[1.097501,0.091491], [1.291552,0.085529], [1.519914,0.079223], [1.788653,0.072813],
	[2.104908,0.066472], [2.477081,0.060321], [2.915059,0.054454], [3.430476,0.048935],
	[4.037025,0.043802], [4.750819,0.039074], [5.590821,0.034755], [6.579345,0.030837],
	[7.742651,0.027303], [9.111645,0.024132], [10.722692,0.021300], [12.618592,0.018779],
	[14.849710,0.016545], [17.475316,0.014570], [20.565161,0.012829], [24.201327,0.011297],
	[28.480411,0.009954], [33.516088,0.008778], [39.442133,0.007751], [46.415973,0.006855],
	[54.622872,0.006075], [64.280848,0.005398], [75.646470,0.004811], [89.021670,0.004303],
	[104.761764,0.003865], [123.284896,0.003488], [145.083138,0.003164], [170.735570,0.002888],
	[200.923659,0.002653], [236.449362,0.002454], [278.256434,0.002288], [327.455496,0.002149],
	[385.353540,0.002035], [453.488650,0.001944], [533.670861,0.001871], [628.030244,0.001815],
	[739.073494,0.001774], [869.750518,0.001746], [1023.532800,0.001730], [1204.505627,0.001723],
	[1417.476611,0.001725], [1668.103410,0.001735], [1963.044021,0.001751],
	[2310.133656,0.001772], [2718.592885,0.001798], [3199.272585,0.001827],
	[3764.942199,0.001860], [4430.628958,0.001895], [5214.017089,0.001932],
	[6135.917601,0.001971], [7220.821137,0.002012], [8497.548578,0.002053],
	[10000.016685,0.002096]])

 
S1=Smilei("ionization_rate1")
S2=Smilei("ionization_rate2")
S3=Smilei("ionization_rate3")

# get electron velocity
ve = np.double(S1.namelist.Species["electron1"].mean_velocity[0])
Ee = (1./np.sqrt(1.-ve**2)-1.)*511. # energy
cse = np.interp(np.log(Ee), np.log(cs[:,0]), cs[:,1]) # interpolate cross section

# get density
ne = np.double(S1.namelist.Species["electron1"].charge_density)

# get ion charge
q0 = np.double(S1.namelist.Species["ion1"].charge)

# Plot simulation result
D1 = S1.ParticleDiagnostic("#0/#1",slice={"x":"all"}, linestyle="None", marker=".", label="We=Wi")
D2 = S2.ParticleDiagnostic("#0/#1",slice={"x":"all"}, linestyle="None", marker=".", label="We=10. Wi")
D3 = S3.ParticleDiagnostic("#0/#1",slice={"x":"all"}, linestyle="None", marker=".", label="We=0.1 Wi")

multiPlot(D1, D2, D3, vmin=q0,skipAnimation=True)

# Plot theory
tmax = np.double(S1.namelist.sim_time)
t = np.linspace(0., tmax, 100)
q = q0 + 1. - np.exp(-ve*ne*cse*t)
plt.plot(t,q,'-k',label="Theory")
plt.legend(loc="best", prop={'size':10})
plt.gca().set_xlabel("Time (normalized)")
plt.gca().set_ylabel("Charge state")
plt.gca().set_title("")

