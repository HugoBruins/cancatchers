import matplotlib.pyplot as plt
import matplotlib.animation

plt.close()


#function that gets the second to last line
def regelKrijgen():
    with open('temperatuurDruk.txt', 'r') as f:
        lines = f.read().splitlines()
        regel = lines[-2]
    return regel

#function that splits the line read into the different variables
def waarde(regel,tijds =[],temps = [],druks =[],hoogtes=[]):
    arraytje = (str(regel)).split("X")
    if len(arraytje) == 6:
        time = float(arraytje[1]) / 1000
        temperatuur = arraytje[2]
        druk = arraytje[3]
        hoogte = arraytje[4]
        return float(time),float(temperatuur),float(druk),float(hoogte)
    else:
        print("crisis man")
        return tijds[-1],temps[-1],druks[-1],hoogtes[-1]


fig, ax = plt.subplots(3)


tijds = []
druks = []
temps = []
hoogtes = []


#animation function for both subplots
def animate(i):
    if tijds:
        tijd,temperatuur,druk,hoogte = waarde(regelKrijgen(),tijds,temps,druks,hoogtes)
    else:
        tijd,temperatuur,druk,hoogte = waarde(regelKrijgen())
    
    tijds.append(tijd)
    temps.append(temperatuur)
    druks.append(druk)
    hoogtes.append(hoogte)
    
    ax[0].clear()
    ax[0].plot(tijds, temps,label = 'temperatuur', color = 'r')
    ax[0].set_xlabel('tijd')
    ax[0].set_ylabel('temperatuur')
    ax[0].grid()
    ax[1].clear()
    ax[1].plot(tijds, druks,label = 'druk', color = 'hotpink')
    ax[1].set_xlabel('tijd')
    ax[1].set_ylabel('druk')
    ax[1].grid()
    ax[2].clear()
    ax[2].plot(tijds, hoogtes,label = 'hoogte',color = 'b')
    ax[2].set_xlabel('tijd')
    ax[2].set_ylabel('hoogte in meters')
    ax[2].grid()
   


#showing animation
ani = matplotlib.animation.FuncAnimation(fig, animate)

plt.show()


