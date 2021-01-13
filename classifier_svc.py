import cv2
import pickle
import numpy as np

cam = cv2.VideoCapture(0)
classifier_file = open("./classifier/svc.pk",'rb')
classifier_svc = pickle.load(classifier_file)

while(True):
    ret,frame = cam.read()
    if ret:
        gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        thresh, thresh_frame = cv2.threshold(gray_frame,127,255,cv2.THRESH_OTSU)
        invert_frame = 255 - thresh_frame
        resize_frame = cv2.resize(invert_frame,(28,28))
        assert(resize_frame.shape == (28,28))
        cv2.imshow("video stream",resize_frame)

    k = cv2.waitKey(1)
    if k & 0xFF == ord('q'):
        break

    elif k & 0xFF == ord('c'):
        resize_frame_np = np.resize(resize_frame,(1,784))
        pred = classifier_svc.predict(resize_frame_np)
        print(f"Prediction {pred}")


cam.release()
cv2.destroyAllWindows