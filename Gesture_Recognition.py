# Importing libraries
import cv2
import numpy as np
import pandas as pd

# Loading weights and biases (that were saved from TensorFlow model)
df1 = pd.read_csv("w1.csv")
df2 = pd.read_csv("w2.csv")
df3 = pd.read_csv("w3.csv")
df4 = pd.read_csv("w4.csv")
df5 = pd.read_csv("w5.csv")
df6 = pd.read_csv("b1.csv")
df7 = pd.read_csv("b2.csv")
df8 = pd.read_csv("b3.csv")
df9 = pd.read_csv("b4.csv")
df10 = pd.read_csv("b5.csv")

w1 = np.array(df1).T[1:].T
w2 = np.array(df2).T[1:].T
w3 = np.array(df3).T[1:].T
w4 = np.array(df4).T[1:].T
w5 = np.array(df5).T[1:].T
b1 = np.reshape(np.array(df6).T[1:], (16,))
b2 = np.reshape(np.array(df7).T[1:], (16,))
b3 = np.reshape(np.array(df8).T[1:], (16,))
b4 = np.reshape(np.array(df9).T[1:], (16,))
b5 = np.reshape(np.array(df10).T[1:], (4,))

# Webcamera setup        
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH,720)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT,480)

_, frame = cap.read()

h, w, c = frame.shape

# Some useful variable initializations
aweight = 0.5
num_frames = 0
num = 0
bg = None
hand_present = 0

# Rectified Linear Unit activation function
def relu(x):
    y = []
    for i in x:
        if i > 0:
            y.append(i)
        else:
            y.append(0)
    y = np.array(y)
    return y

# Softmax activation function
def softmax(x):
    e_x = np.exp(x - np.max(x))
    return e_x / e_x.sum(axis=0)

# Function to predict the gesture provided an input image
# Basically a neural network inference engine
def predict(x0):
    x1 = x0.flatten()
    x2 = relu(np.matmul(x1, w1) + b1)
    x3 = relu(np.matmul(x2, w2) + b2)
    x4 = relu(np.matmul(x3, w3) + b3)
    x5 = relu(np.matmul(x4, w4) + b4)
    x6 = softmax(np.matmul(x5, w5) + b5) 
    return x6

# Function to average out some images (used to capture background)
def run_avg(img, aweight):
    global bg
    if bg is None:
        bg = img.copy().astype('float')
        return
    cv2.accumulateWeighted(img, bg, aweight)

# Function to segment image by subracting captured background 
def segment(img, thres=15):
    global bg
    diff = cv2.absdiff(bg.astype('uint8'), img)
    _, thresholded = cv2.threshold(diff,thres, 255, cv2.THRESH_BINARY)
    contours,_ = cv2.findContours(thresholded.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if len(contours) == 0:
        return
    else:
        segmented = max(contours, key = cv2.contourArea)
    return (thresholded, segmented)

# Forever loop
while True:
    _, frame = cap.read()
            
    frame = cv2.flip(frame, 1)
    clone = frame.copy()
    (height, width) = frame.shape[:2]
    roi = frame[200:400, 200:400]
    gray = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (7, 7), 0)

    if num_frames < 50:
        run_avg(gray, aweight)
    elif num_frames == 50:
        print("Background captured!")
        hand = segment(gray)
    else:
        hand = segment(gray)
    
        if hand is not None:
            hand_present = 1
            (thresholded, segmented) = hand
            num = num + 1
            cv2.drawContours(clone, [segmented + (300, 100)], -1, (0, 0, 255))
            cv2.imshow("Thesholded", thresholded)
            input_img = cv2.resize(thresholded, (16,16))
            prediction = predict(input_img)
            max_val = prediction[0]
            max_ind = 0
            for l in range(1,4):
                if prediction[l] > max_val:
                    max_val = prediction[l]
                    max_ind = l
            if max_ind == 0:
                text = "Fist"
            elif max_ind == 1:
                text = "One"
            elif max_ind == 2:
                text = "Palm"
            else:
                text = "Peace"
        else:
            hand_present = 0
            
    num_frames += 1
    if hand_present == 0:
        dummy = 0
    else:
        cv2.putText(roi, text, (50,50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2, cv2.LINE_AA)
    cv2.imshow("ROI", roi)
    cv2.imshow("Frame", frame)
    
    if cv2.waitKey(20) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

