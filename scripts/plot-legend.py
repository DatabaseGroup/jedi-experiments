import matplotlib.pyplot as plt
import math
import pylab

# Force matplotlib to use TrueType (42) fonts.
plt.rc('pdf',fonttype = 42)
plt.rc('ps',fonttype = 42)

labels = ['Scan, Baseline','Scan, QuickJEDI','Scan, Baseline, Wang','Scan, QuickJEDI, Wang','Scan, Baseline, JOFilter','Scan, QuickJEDI, JOFilter','JSIM, Baseline','JSIM, QuickJEDI','JSIM, Baseline, Wang','JSIM, QuickJEDI, Wang','JSIM, Baseline, JOFilter','JSIM, QuickJEDI, JOFilter']

linestyles = ['-'] #, '-.', '--', ':']
colors = ['#ca6680', '#e8a77c', '#946592', '#4f6ece', '#e8a77c', '#63a375',
            '#ca6680', '#e8a77c', '#946592', '#4f6ece', '#e8a77c', '#78c0dd']
markers = ['.', '.', '.', '.', '.', 'X',
            '*', 'p', '.', '.', '.', 'D']
markersizes = [0, 0, 0, 0, 0, 15,
            20, 20, 30, 0, 0, 15]

fig = pylab.figure()
figlegend = pylab.figure(figsize=(10.7,0.3))
ax = fig.add_subplot(111)
lines = []
ex = [0, 1, 2, 3, 4, 9, 10]
used_labels = []

for i in range(len(colors)*len(linestyles)):
    if i not in ex:
        print(str(i) + " : " + labels[i])
        used_labels.append(labels[i])
        
        t = ax.plot(range(10), pylab.randn(10), linewidth=3, label = labels[i], 
                        linestyle=linestyles[i % len(linestyles)], 
                        color=colors[i % len(colors)], 
                        marker=markers[i % len(markers)],
                        markersize=markersizes[i % len(markersizes)]-5)[0]
        lines.append(t)

figlegend.legend(lines, used_labels, ncol=5, frameon=False)
fig.show()
figlegend.show()
figlegend.savefig('legend.eps')