import serial
import time
import select

# Configuració de la porta sèrie
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

# Llegir les dades de la porta sèrie durant 10 segons
MAX_TIME = 60 # temps màxim en segons
start_time = time.time()  # temps d'inici de la lectura
with open('dades1.txt', 'w') as f:
    while (time.time() - start_time) < MAX_TIME:
        if ser.in_waiting:  
            line = ser.readline().decode('utf-8')
            f.write(line)
            print(line, end='')
        else:
            # Esperar fins que hi hagi dades disponibles o s'hagi superat el temps màxim
            rlist, _, _ = select.select([ser], [], [], MAX_TIME - (time.time() - start_time))
            if ser in rlist:
                line = ser.readline().decode('utf-8')
                f.write(line)
                print(line, end='')

# Tancar la connexió amb la porta sèrie
ser.close()