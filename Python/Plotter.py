import serial
import time
import numpy as np
import matplotlib.pyplot as plt

# Configuració del port sèrie
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
ser.flush()

# Crear la figura
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

# Inicialitzar les dades
xs = []
ys = []
start_time = time.time()

# Bucle principal
while True:
    # Llegir les dades del port sèrie
    try:
        line = ser.readline().decode('utf-8').rstrip()
        data = float(line)
    except:
        continue
    
    # Afegir les dades a la llista
    xs.append(time.time() - start_time)
    ys.append(data)
    
    # Esborrar les dades antigues
    while xs[-1] - xs[0] > 10:
        xs.pop(0)
        ys.pop(0)
    
    # Actualitzar la gràfica
    ax.clear()
    ax.plot(xs, ys)
    ax.set_ylim([min(ys)-0.5, max(ys)+0.5])
    ax.set_xlabel('Temps (s)')
    ax.set_ylabel('Dades')
    ax.set_title('Gràfica en temps real')
    plt.pause(0.01)
