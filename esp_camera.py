import cv2

cap = cv2.VideoCapture()
cap.open("http://192.168.4.1:81/stream")


while True:
	ret, frame = cap.read()
	if ret:
		cv2.imshow("stream",frame)
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break

cap.release()
cv2.destroyAllWindows()

