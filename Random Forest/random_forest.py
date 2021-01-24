import cv2
import pickle
import numpy as np

DEBUG = 0
FILE_NAME = "test_"
count = 0
# cam = cv2.VideoCapture(0)
classifier_file = open("random_forest.pk",'rb')
classifier_rf = pickle.load(classifier_file)

if DEBUG:
    cam = cv2.VideoCapture(0)
else:
    cam = cv2.VideoCapture()
    cam.open("http://192.168.4.1:81/stream")

while(True):
    ret,frame = cam.read()
    if ret:
        img_rgb = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
        cv2.imshow("video stream",frame)

    k = cv2.waitKey(1)
    if k & 0xFF == ord('q'):
        break

    elif k & 0xFF == ord('c'):
        resize_frame_np =  np.reshape(img_rgb,(1, img_rgb.shape[0] * img_rgb.shape[1] * img_rgb.shape[2]))
        print("######## PREDICTION ###########")
        pred = classifier_rf.predict(resize_frame_np)
        print(f"Prediction : {pred}")
    
    elif k & 0xFF == ord('p'):
        cv2.imwrite(FILE_NAME + str(count) + ".jpg",frame)
        count += 1


cam.release()
cv2.destroyAllWindows