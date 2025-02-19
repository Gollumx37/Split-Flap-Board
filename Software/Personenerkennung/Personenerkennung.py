import cv2
import numpy as np
import urllib.request
import time
from imutils.object_detection import non_max_suppression

# ESP32-CAM Stream-URL (statt /capture, da dies nicht mehr funktioniert)
ESP32_STREAM_URL = "http://192.168.148.209:81/stream"

# HOG-Detektor initialisieren
hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

def nothing(x):
    pass

# Trackbars zur Live-Anpassung
cv2.namedWindow("Settings")
cv2.createTrackbar("winStride", "Settings", 2, 10, nothing)  # Präzisere Erkennung mit kleinerem Schritt
cv2.createTrackbar("padding", "Settings", 8, 20, nothing)   # Standardwert verbessert Präzision
cv2.createTrackbar("scale", "Settings", 15, 50, nothing)    # 1.5 als Startwert für genauere Detektion

def get_frame():
    """ Holt ein Einzelbild aus dem MJPEG-Stream des ESP32-CAM """
    try:
        stream = urllib.request.urlopen(ESP32_STREAM_URL)
        bytes_data = b""
        while True:
            bytes_data += stream.read(1024)
            a = bytes_data.find(b'\xff\xd8')  # Start Marker für JPEG
            b = bytes_data.find(b'\xff\xd9')  # End Marker für JPEG
            if a != -1 and b != -1:
                jpg = bytes_data[a:b+2]
                bytes_data = bytes_data[b+2:]
                return cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
    except Exception as e:
        print(f"Fehler beim Abrufen des Bildes: {e}")
        return None

while True:
    frame = get_frame()
    if frame is None:
        continue
    
    # Bildgröße anpassen
    frame = cv2.resize(frame, (800, 600))  # Größere Auflösung für detailliertere Erkennung
    
    # Bild in Graustufen konvertieren
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Kontrast verbessern
    gray = cv2.equalizeHist(gray)

    # Rauschen reduzieren für weniger Fehlalarme
    gray = cv2.GaussianBlur(gray, (5, 5), 0)

    # Werte aus Trackbars holen
    winStride = cv2.getTrackbarPos("winStride", "Settings") or 2
    padding = cv2.getTrackbarPos("padding", "Settings") or 8
    scale = cv2.getTrackbarPos("scale", "Settings") / 10.0
    if scale < 1.05:
        scale = 1.05  # Mindestwert für sinnvolle Skalierung

    # Personen erkennen
    rects, weights = hog.detectMultiScale(
        gray,  # Graustufenbild verwenden
        winStride=(winStride, winStride),
        padding=(padding, padding),
        scale=scale
    )

    # Gewichtungsschwelle setzen, um Fehlalarme zu reduzieren
    threshold = 0.7  # Je höher, desto weniger Fehlalarme
    rects = [r for i, r in enumerate(rects) if weights[i] > threshold]

    # Non-Maxima-Suppression (reduziert doppelte Erkennungen)
    rects = np.array([[x, y, x + w, y + h] for (x, y, w, h) in rects])
    pick = non_max_suppression(rects, probs=None, overlapThresh=0.5)  # Senkt die Schwelle für genauere Detektion

    # Erkennungen zeichnen
    for (xA, yA, xB, yB) in pick:
        cv2.rectangle(frame, (xA, yA), (xB, yB), (0, 255, 0), 2)

    # Personenanzahl anzeigen mit weißem Hintergrund und schwarzer Schrift
    text = f"Personen: {len(pick)}"
    (text_width, text_height), baseline = cv2.getTextSize(text, cv2.FONT_HERSHEY_SIMPLEX, 1, 2)
    cv2.rectangle(frame, (10, 10), (10 + text_width, 50), (255, 255, 255), -1)
    cv2.putText(frame, text, (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)

    # Bild anzeigen
    cv2.imshow("ESP32-CAM Personenerkennung", frame)
    
    # Warte 0,2 Sekunden für stabilere Ergebnisse
    time.sleep(0.4)

    # Beenden mit 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
