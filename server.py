from flask import Flask
from flask import request

app = Flask(__name__)

@app.route("/")
def handle_sensor_data():
    # creating a tempVal and humVal variable to hold the data values for temperature and humidity detected
    tempVal = request.args.get("tempVal")
    humVal = request.args.get("humVal")

    if tempVal and humVal:
        print(f"Current Temperature: {tempVal} °C\nCurrent Humidity: {humVal} %")
        print("-"*30)
        return f"Current Temperature: {tempVal} °C\nCurrent Humidity: {humVal} %"
    else:
        # for debugging purposes to ensure there are actually values being detected and the connection is working
        print("No temperature or humidity values detected")
        return "No temperature or humidity values detected", 400  # Returns a 400 Bad Request
