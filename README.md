# Hand-Gesture-Recognition

This project implements hand gesture recognition using 2 different approaches. One is using EMG signals and the other is using images. </br>
4 gestures are recognized - 1) Fist/Rest, 2) One, 3) Palm/Five, 4) Peace/Two. </br>

# EMG based recognition: 

Dataset is created using TIVA Embedded development board and 2 EMG Sensors from Upside Down Labs.Analog signals from sensors are converted using two inbuilt ADCs in 
TIVA microcontrollers. These are then send to computer via UART interface.</br>
Link to EMG dataset: </br>
https://drive.google.com/drive/folders/1FuZavpRWnWinDOZcZ21C-xJbrTWOWpAX?usp=share_link </br>

EMG_Model.ipynb can be run on Google Colab, hence it has no dependecies. Dataset must be uploaded on Google Drive first. </br>

# Image based recognition:

Dataset is created using OpenCV based hand extraction from live video feed from web-camera. </br>
Link to image dataset: </br>
https://drive.google.com/file/d/1HVX-onPxnU2i90w0JwQqQnkFg2eCLtYn/view?usp=share_link </br>

Neural_Network.ipynb can be run on Google Colab, hence it has no dependencies. Dataset must be uploaded on Google Drive first. </br>
Gesture_Recognition.py has to be run on a computer with a webcamera. The weight and bias csv files generated by Neural_Network.ipynb must be present in the same directory as Gesture_Recognition.py. Dependencies are NumPy, OpenCV (cv2) and Pandas. </br>
To extract the hand, background subtraction has been used. The background must be same throughout in order for this to work. </br>

These projects are only prototypes for illustrating hand gesture recognition. Typically much more diverse data from more number of EMG sensors is needed in order to build an EMG based gesture recognition model that works in a real world scenario. As for images, more complicated models such as Convolutional Neural Networks are known to perform better. 
