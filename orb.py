import cv2
import matplotlib.pyplot as plt

DEBUG = 0

if DEBUG:
	cap = cv2.VideoCapture(0)
else:
	cap = cv2.VideoCapture()
	cap.open("http://192.168.4.1:81/stream")
orb = cv2.ORB_create()



while(True):
    ret,frame = cap.read()
    if ret:
        frame = cv2.flip(frame,1)
        kp = orb.detect(frame,None)
        kp, des = orb.compute(frame, kp)
        kp_frame = cv2.drawKeypoints(frame, kp, None, color=(0,255,0), flags=0)
        cv2.imshow("stream",kp_frame)

    k = cv2.waitKey(1) & 0xFF

    if k == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()





