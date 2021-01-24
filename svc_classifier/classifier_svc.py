import cv2
import pickle
import numpy as np

DEBUG = 0

# cam = cv2.VideoCapture(0)
classifier_file = open("svc.pk",'rb')
classifier_svc = pickle.load(classifier_file)

if DEBUG:
    cam = cv2.VideoCapture(0)
else:
    cam = cv2.VideoCapture()
    cam.open("http://192.168.4.1:81/stream")

while(True):
    ret,frame = cam.read()
    if ret:
        img_rgb = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
        gray_frame = cv2.cvtColor(img_rgb,cv2.COLOR_BGR2GRAY)
        thresh, thresh_frame = cv2.threshold(gray_frame,127,255,cv2.THRESH_OTSU)
        invert_frame = 255 - thresh_frame
        resize_frame = cv2.resize(invert_frame,(28,28))
        assert(resize_frame.shape == (28,28))
        cv2.imshow("video stream",invert_frame)

    k = cv2.waitKey(1)
    if k & 0xFF == ord('q'):
        break

    elif k & 0xFF == ord('c'):
        resize_frame_np = np.resize(resize_frame,(1,784))
        print("######## PREDICTION ###########")
        for i in range(len(classifier_svc)):
            pred = classifier_svc[i].predict(resize_frame_np)
            print(f"Prediction {i} : {pred}")


cam.release()
cv2.destroyAllWindows