
from imutils.video import VideoStream
from threading import Thread
from keras.models import load_model
from keras.applications.mobilenet_v2 import preprocess_input
import imutils
import cv2
import numpy as np
import serial
import time
import os

MODEL_PATH = 'models/model_final_3dir_relu2_v0016_t002.hdf5'

# Donnees pour la connexion a l'arduino
com_arduino = '/dev/ttyACM0'
baudrate_arduino = 9600

# Limite de temps d'execution du programme
max_working_time = 150 # in seconds
intervalle = 10 # in 100th of second
# Pause avant lancement
dodo = 3

# Encoder les int en bytes
def my_encode(x):
    if x == 0:
        return b'0'
    elif x == 1:
        return b'1'
    elif x == 2:
        return b'2'
    else:
        return b'3'

# On attend l'arduino avant de continuer
# Pour rester synchronise avec elle
def wait_for_arduino(ser):
    while ser.inWaiting() < 0:
        time.sleep(.1)
    time.sleep(.1)
    arData = str(ser.readline())

# Creation du masque
dark = np.ones((224, 224, 3))
for i in range(int(224*0.45)):
    dark[i] = np.ones((224, 3))

# loading the model
print("[INFO] loading model...")
model = load_model(MODEL_PATH)

# initialize the video stream and allow the camera sensor to warm up
print("[INFO] starting video stream...")
vs = VideoStream(usePiCamera=True).start()
time.sleep(dodo)

# Trying to connect
connectionSuccess = False
while not connectionSuccess:
    try:
        ser = serial.Serial(com_arduino, baudrate = baudrate_arduino, timeout = 1)
        connectionSuccess = True
    except (OSError, serial.SerialException):
        sleep(.1)
        pass

print('Connected')

ser.flushInput()

print('Go !')

# Repere pour les intervalles
start = int(round(time.time()))
start2 = int(round(time.time()*100))

while time.time() - start < max_working_time:
    
    # Waiting 2 seconds because the first datas are corrupted
                                # At least "intervalle" 100th of second between each capture
    if (time.time() - start >= 2) and (int(round(time.time()*100)) > (start2 + intervalle)):

        frame = vs.read() # Prise de la photo

        # On resize
        frame = imutils.resize(frame, height = 224)
        frame = frame[:, 37:-37, :]

        # On applique le filtre
        frame = frame*dark

        # Preprocessing
        frame = preprocess_input(frame)
        # (224, 224, 3) -> (1, 224, 224, 3)
        # car .predict() ne prend que des tenseurs a 4 dimensions
        frame = np.expand_dims(frame, axis = 0)

        # Orediction
        pred = model.predict(frame)[0]
        pred2 = int(pred)
        print(pred, pred2)

        # On l'envoie a l'arduino
        ser.write(my_encode(pred2))

        # On s'assure de rester synchro
        wait_for_arduino(ser)

        # Repere pour les intervalles
        start2 = int(round(time.time()*100))
      

start -= 2

# on ferme le tout proprement
print("[INFO] cleaning up...")
cv2.destroyAllWindows()
vs.stop()

























