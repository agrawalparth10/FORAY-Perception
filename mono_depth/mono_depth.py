import cv2

DEBUG = 1
object_length = 0.228
pixel_length = 500
distance = 0.51
i0 = 720 // 2
j0 = 1280 // 2

def x_hat(olength,ip,i0,pflength):
	return (olength * (ip - i0)) / pflength
	
def y_hat(olength,jp,j0,pflength):
	return (olength * (jp-j0)) / pflength

def z_hat(d,plength,pflength):
	return d*(plength/pflength)

def cal_coordinates(d,olength,ip,jp,i0,j0,pflength,plength):
	return x_hat(olength,ip,i0,pflength),y_hat(olength,jp,j0,pflength),z_hat(d,plength,pflength)


def measure(d,olength,ip,jp,i0,j0,pflength,plength):
	x_cor = x_hat(olength,ip,i0,pflength)
	z_cor = z_hat(d,plength,pflength)
	distance = (x_cor ** 2 + z_cor ** 2) ** 0.5
	return distance
"""
if DEBUG:
	cap = cv2.VideoCapture(0)
else:
	cap = cv2.VideoCapture()
	cap.open("http://192.168.4.1:81/stream")	
"""

cap = cv2.VideoCapture()
cap.open("http://192.168.4.1:81/stream")

face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

while(True):
	ret,frame = cap.read()
	if ret:
		gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
		faces = face_cascade.detectMultiScale(gray_frame)
		if len(faces) > 0:
			x,y,w,h = faces[0]
			frame = cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)
			if DEBUG:
				faceROI = frame[y:y+h,x:x+w]
				print(faceROI.shape)
				print(frame.shape)
			center_frame = frame[60:660,340:940]
			
			# CALCULATE DISTANCE
			ip = x
			jp = y
			pflength = w
			relative_distance = measure(distance,object_length,ip,jp,i0,j0,pflength,pixel_length)
			print(f"Relative Distance {relative_distance}")
			cv2.imshow("stream",center_frame)
		
	k = cv2.waitKey(1) & 0xFF
	if k == ord('q'):
		break


cap.release()
cv2.destroyAllWindows()

