
from imutils.video import VideoStream
from threading import Thread
import imutils
import cv2
import numpy as np
import time
import os

fichier = "data/..." # fichier ou sauvegarder les donnees

# temps max de fonctionnement du programme
max_record_time = 600 # in seconds
# intervalle de sauvegarde des donnees
intervalle = 10 # in 100th of second
# attente pour avoir le temps de preparer la voiture
dodo = 30
# nombre de donnees a prendre
nb_imgs = 3000

# initialize the video stream and allow the camera sensor to warm up
print("[INFO] starting video stream...")
vs = VideoStream(usePiCamera=True).start()
time.sleep(dodo)

compteur = 0 # pour ne pas se perdre dans la boucle while
print('Go !')
start = int(round(time.time())) # plus que max_record_time secondes d'execution

# on enregistre les images dans un ordre aleatoire
order = np.arange(nb_imgs)
np.random.shuffle(order)

# repere pour les intervalles
start2 = int(round(time.time()*100))

while time.time() - start < max_record_time:
    
    # Waiting 2 seconds because the first datas are corrupted
                                # At least "intervalle" 100th of second between each capture
    if (time.time() - start >= 2) and (int(round(time.time()*100)) > (start2 + intervalle)):
        
        frame = vs.read() # prise de la photo

        # on resize
        frame = imutils.resize(frame, height = 224)
        frame = frame[:, 37:-37, :]

        # on enregistre
        cv2.imwrite("{}/{}.png".format(fichier, int(order[compteur])), frame.astype("uint8"))

        # pour ne pas se perdre dans le comptage des donnees
        compteur += 1

        # a "nb_imgs" donnees on arrete la prise de donnees
        if compteur >= nb_imgs:
            break

        # repere pour les intervalles
        start2 = int(round(time.time()*100))
      

start -= 2

# Printing some infos
print("Time : " + str((int(round(time.time()*100)) - start*100) / 100) +
                                                      "s     Nb data : " + str(compteur) )
print( str(round((float((round(time.time()*100)) - start*100) / 100.) / compteur, 3))
                                                                       + " seconds per data")

# on ferme le tout proprement
print("[INFO] cleaning up...")
cv2.destroyAllWindows()
vs.stop()






