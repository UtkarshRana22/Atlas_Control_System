import requests
import time
import math

ESP32_IP = "http://192.168.4.1"  # ESP32 Access Point IP

def send_xy(x, y):
    try:
        url = f"{ESP32_IP}/joystick?x={x}&y={y}"
        r = requests.get(url, timeout=1)
        print(f"Sent X={x}, Y={y} | ESP32 replied: {r.text}")
    except Exception as e:
        print("Error:", e)

# Generate pseudo joystick values
for angle in range(0, 360, 30):  # every 30 degrees
    x = round(math.cos(math.radians(angle)) * 100, 2)
    y = round(math.sin(math.radians(angle)) * 100, 2)
    send_xy(x, y)
    time.sleep(1)  # wait before next value
