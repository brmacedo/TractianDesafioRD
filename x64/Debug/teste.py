import pandas as pd
import matplotlib.pyplot as plt
from scipy.fft import fft, ifft
from matplotlib.pyplot import figure

df = pd.read_csv('output.txt', sep=",", header=None)
df.columns = ["x", "y", "z", "freq"]

plt.plot(df['freq'], df['x'], 'tab:blue')
plt.plot(df['freq'], df['y'], 'tab:orange')
plt.plot(df['freq'], df['z'], 'tab:green')

figure(figsize=(80, 60), dpi=70)

fig, axs = plt.subplots(2, 2)
axs[0, 0].plot(df['freq'], df['x'], 'tab:blue')
axs[0, 0].set_title('X')
axs[0, 1].plot(df['freq'], df['y'], 'tab:orange')
axs[0, 1].set_title('Y')
axs[1, 0].plot(df['freq'], df['z'], 'tab:green')
axs[1, 0].set_title('Z')
axs[1, 1].plot(df['freq'], df['x'], 'tab:blue')
axs[1, 1].plot(df['freq'], df['y'], 'tab:orange')
axs[1, 1].plot(df['freq'], df['z'], 'tab:green')
axs[1, 1].set_title('XYZ')

for ax in axs.flat:
    ax.set(xlabel='x-label', ylabel='y-label')

# Hide x labels and tick labels for top plots and y ticks for right plots.
for ax in axs.flat:
    ax.label_outer()
