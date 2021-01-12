"""
To Capture Images from Webcam
Usage - 
DIR {String} - Set the path of the folder
FILE_NAME {String} - Filename of the images 

Press 'c' to Capture
Press 'q' to Quit

"""



import cv2 

CAPTURE = False
DIR = "./dataset/"
FILE_NAME = "red_"

cam = cv2.VideoCapture(0)
count = 0 

while(True):
    ret,frame = cam.read()
    if ret:
        cv2.imshow("stream",frame)

    if CAPTURE:
        if not cv2.imwrite(DIR + FILE_NAME + str(count) + ".jpg",frame):
            print("[ERROR] Image not captured")
            exit()
        print(f"Image Captured Number {count}")
        count += 1
        CAPTURE = False

    k = cv2.waitKey(1)
    if k & 0xFF == ord('q'):
        break

    elif k & 0xFF == ord('c'):
        CAPTURE = True



cam.release()
cv2.destroyAllWindows()