# A novel engine for BCI
# 
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from scipy.fftpack import fft, ifft

REFRESH_RATE = 120

def stim(f_stimulus, n_frame):
    y = (1 + np.sin(2*np.pi*f_stimulus*(n_frame/REFRESH_RATE)))/2
    return y

x = np.linspace(0, 1, REFRESH_RATE*50)
y = [0 for i in range(len(x))]
f_stim = 8
for n in range(REFRESH_RATE*50):
    y[n] = stim(f_stim, int(n/50))

my_x_ticks = np.arange(0, 1, 0.1)
plt.xticks(my_x_ticks)
plt.axis([0,1, 0,1])

plt.grid(True, which='both', ls='dashed')
plt.plot(x, y, c='b', linewidth=2.0)
font2 = {'family' : 'Times New Roman',
'weight' : 'normal',
'size'   : 18,
}
plt.xlabel('Time/s',font2)
plt.ylabel('Luminance',font2)
# plt.title('Stimulated stimulus signal')
plt.show()




