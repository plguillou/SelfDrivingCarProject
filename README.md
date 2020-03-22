# SelfDrivingCarProject
Building a self-driving car from scratch using DeepLearning

Here are the principal files used in our research project in Computer Science and Electronics

## PDF files
...RAPPORT... is our research report (written in French)
...PRESENTATION... is our PowerPoint for the oral presentation of the project

## car_files
Contains the files loaded on the self-drving car

Joysticks: Arduino code of our remote controller
Moteurs_autonomes: Arduino code of our motors
(in the model folder, the stop file contain the models trained to recognize a "stop" sign, a "prohibited direction" sign and a human)
capture_data: Python code to capture the data with the joysticks and the camera from the pov of the car
self_driving: Python code to let the car drive autonomously with a trained model

## training_mobilenet
Contains the files used to handle the data (we used Google Colab)

MobileNet_v2_training: IPython Notebookfile file with code to train the CNN
data_aug: IPython Notebook file with code to flip the data and apply a dark mask on the upper side of the data so the CNN can focus on the road
