import requests
import time

esp32_ip = "http://192.168.4.1"

# Wait until ESP32 responds
max_attempts = 10
for attempt in range(max_attempts):
    try:
        resp_get = requests.get(f"{esp32_ip}/getData", timeout=1)
        print("GET response:", resp_get.text)
        break  # success
    except requests.exceptions.RequestException:
        print("ESP32 not ready yet, retrying...")
        time.sleep(1)
else:
    print("Failed to connect to ESP32 after retries.")
    exit()

# POST request
for attempt in range(max_attempts):
    try:
        resp_post = requests.post(
            f"{esp32_ip}/postData",
            data="on",
            headers={"Content-Type": "text/plain"},
            timeout=1
        )
        print("POST response:", resp_post.text)
        break
    except requests.exceptions.RequestException:
        print("ESP32 not ready for POST, retrying...")
        time.sleep(1)
