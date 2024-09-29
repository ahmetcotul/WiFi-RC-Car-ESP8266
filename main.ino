#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define BUZZER_PIN D0

// Global speed and rotation parameters
int mainSpeed;
int defaultTurnDifference = 20;
int currentTurnDifference = 20;

int rightSpeedPin = D2;
int forwardPinRight = D3;
int backwardPinRight = D4;

int leftSpeedPin = D7;
int forwardPinLeft = D6;
int backwardPinLeft = D5;

int ledPin = D8;

// WiFi credentials
const char* ssid = "MySSID";  // CHANGE IT
const char* password = "12345678";  // CHANGE IT

// Web server object
ESP8266WebServer server(80);

// HTML content to serve
const char* htmlContent = R"=====(
<!DOCTYPE html>

<html lang="en">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">

<meta charset="UTF-8">
    <style>
        html{
          background-color:black;
        }
        body {
            font-family: Arial, sans-serif;
            text-align: center;
        }
        h1 {
            color: #333;
        }
        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            margin-bottom: 20px;
        }
        .row {
            display: flex;
            justify-content: center;
            margin-bottom: 10px;
        }
        .button {
            padding: 40px; /* Increased button size */
            font-size: 40px; /* Increased font size */
            border: 2px solid #007bff;
            border-radius: 5px;
            background-color: #007bff;
            color: #fff;
            cursor: pointer;
            transition: background-color 0.3s ease;
            margin: 5px;
            outline: none; /* Removed default focus on buttons */
        }
        .button:active {
            transform: scale(0.9); /* Scaled down button on click */
        }
        .button:hover {
            background-color: #0056b3;
        }
        #turboButton {
            background-color: #ff6600;
            color: #fff;
        }
        #stopButton {
            background-color: #ff0000;
        }
        #northButton, #southButton, #rightButton , #leftButton {
            background-color : #fa00ff;
        }
		#hornButton {
			background-color :  #ff7875;
		}
    </style>
</head>
<body>
    <h1 style="color:white;" >CAR_NAME</h1>
    <div class="container">
        <button id="turboButton"  onmousedown="pressed('turbo')" onmouseup="released('turbo')" class="button" name='turbo' value='on'>Turbo</button>
        <div class="row">
            <button id="northwestButton" onmousedown="pressed('northwest')" onmouseup="released('northwest')" class="button" name='direction' value='northwest'>↖</button>
            <button id="northButton"  onmousedown="pressed('forward')" onmouseup="released('forward')" class="button" name='direction' value='forward'>↑</button>
            <button id="northeastButton" onmousedown="pressed('northeast')" onmouseup="released('northeast')" class="button" name='direction' value='northeast'>↗</button>
        </div>
        <div class="row">
            <button id="leftButton" onmousedown="pressed('left')" onmouseup="released('left')" class="button" name='direction' value='left'>←</button>
            <button id="stopButton" onmousedown="pressed('stop')" onmouseup="released('stop')" class="button" name='direction' value='stop'>■</button>
            <button id="rightButton"  onmousedown="pressed('right')" onmouseup="released('right')" class="button" name='direction' value='right'>→</button>
        </div>
        <div class="row">
            <button id="southwestButton"  onmousedown="pressed('southwest')" onmouseup="released('southwest')"   class="button" name='direction' value='southwest'>↙</button>
            <button id="southButton" onmousedown="pressed('backward')" onmouseup="released('backward')" class="button" name='direction' value='backward'>↓</button>
            <button id="southeastButton" onmousedown="pressed('southeast')" onmouseup="released('southeast')"  class="button" name='direction' value='southeast'>↘</button>
        </div>
		<div id="output"></div>

        <input type='range' min='0' max='255' step='5' id="slider">
		<output for='slider' style="color:white;" id='volume'>0</output>

        <button id="hornButton"  onmousedown="pressed('horn')" onmouseup="released('horn')" class="button" name='horn' value='on'>Horn</button>
		
		
    </div>
	<script>
		
		var pressedState = {};
		var slider = document.getElementById('slider');
		var volume = document.getElementById('volume');
		var xhr;

		slider.oninput = function() {
			
			var value = this.value ;
			volume.innerHTML = value;
			if(xhr && xhr.readyState != 4){
			    xhr.abort(); // Cancel the previous request
			}
				xhr = new XMLHttpRequest();
				xhr.open('GET', '/update?value=' + value, true);
				xhr.send();
		}

		function pressed(direction){
			  if (!pressedState[direction]) {
                pressedState[direction] = true;
                controlMotor(direction);
            }
		}
		
		
		function released(direction){
            pressedState[direction] = false;
			stopMotor(direction);
		}
		
		function controlMotor(direction){
		  var xhr = new XMLHttpRequest();
		  var url =  direction ;
		  console.log(url);
		  xhr.open("GET", "http://192.168.4.1/" + url, true);
		  xhr.send();
		}

		function stopMotor(direction){
      if(direction == "turbo"){
        var xhr = new XMLHttpRequest();
        url = "release" + direction;
        console.log(url);
        xhr.open("GET", "http://192.168.4.1/" + url, true);
        xhr.send();
      }
		}
	</script>
</body>
</html>

)=====";

// Function to check motor direction (forward, backward, left, or right)
String getMotorDirection() {
    int backwardRightState = digitalRead(backwardPinRight);
    int forwardRightState = digitalRead(forwardPinRight);
    int forwardLeftState = digitalRead(forwardPinLeft);
    int backwardLeftState = digitalRead(backwardPinLeft);

    if (backwardRightState == 1 && backwardLeftState == 1 && forwardRightState == 0 && forwardLeftState == 0) {
        return "backward";
    } else if (backwardRightState == 0 && backwardLeftState == 0 && forwardRightState == 1 && forwardLeftState == 1) {
        return "forward";
    } else if (backwardRightState == 1 && backwardLeftState == 0 && forwardRightState == 0 && forwardLeftState == 1) {
        return "right";
    } else if (backwardRightState == 0 && backwardLeftState == 1 && forwardRightState == 1 && forwardLeftState == 0) {
        return "left";
    } else {
        return "stopped";
    }
}

// Function to handle the root path and serve HTML content
void handleRoot() {
    server.send(200, "text/html", htmlContent);
}

// Function to handle 404 errors (Not Found)
void handleNotFound() {
    server.send(404, "text/plain", "404 Not Found");
}

// Function to update motor speed based on input value
void handleSpeedUpdate() {
    if (server.argName(0) == "value") {
        String valueStr = server.arg(0);
        int value = valueStr.toInt();
        mainSpeed = value;
        analogWrite(rightSpeedPin, mainSpeed);
        analogWrite(leftSpeedPin, mainSpeed);
    }
    server.send(200, "text/plain", "OK");
}

void setup() {
    // Initialize motor pins as output
    pinMode(forwardPinRight, OUTPUT);
    pinMode(backwardPinRight, OUTPUT);
    pinMode(rightSpeedPin, OUTPUT);
    pinMode(forwardPinLeft, OUTPUT);
    pinMode(backwardPinLeft, OUTPUT);
    pinMode(leftSpeedPin, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(ledPin, OUTPUT);

    Serial.begin(115200);

    // Start ESP8266 in AP mode
    WiFi.softAP(ssid, password);
    Serial.println("AP Mode started");

    // Print the IP address
    Serial.println("ESP8266 IP Address:");
    Serial.println(WiFi.softAPIP());

    // Set up server routes
    server.on("/", HTTP_GET, handleRoot);

    // Turbo mode route
    server.on("/turbo", HTTP_GET, []() {
        digitalWrite(rightSpeedPin, HIGH);
        digitalWrite(leftSpeedPin, HIGH);
        Serial.println("Turbo mode activated...");
        server.send(200, "text/plain", "Turbo mode.");
    });
    server.on("/releaseTurbo", HTTP_GET, []() {
        analogWrite(rightSpeedPin, mainSpeed);
        analogWrite(leftSpeedPin, mainSpeed);
        Serial.println("Turbo mode deactivated...");
        server.send(200, "text/plain", "Turbo deactivated.");
    });

    // Forward movement
    server.on("/forward", HTTP_GET, []() {
        currentTurnDifference = defaultTurnDifference;
        analogWrite(rightSpeedPin, mainSpeed);
        analogWrite(leftSpeedPin, mainSpeed);

        Serial.println("Moving forward...");
        digitalWrite(backwardPinRight, LOW);
        digitalWrite(forwardPinRight, HIGH);
        digitalWrite(backwardPinLeft, LOW);
        digitalWrite(forwardPinLeft, HIGH);
        digitalWrite(ledPin, HIGH);
        server.send(200, "text/plain", "Moving forward.");
    });
    server.on("/releaseForward", HTTP_GET, []() {
        Serial.println("Forward movement stopped...");
        server.send(200, "text/plain", "Forward stopped.");
    });

    // Backward movement
    server.on("/backward", HTTP_GET, []() {
        currentTurnDifference = defaultTurnDifference;
        analogWrite(rightSpeedPin, mainSpeed);
        analogWrite(leftSpeedPin, mainSpeed);
        Serial.println("Moving backward...");
        delay(500);
        digitalWrite(backwardPinRight, HIGH);
        digitalWrite(forwardPinRight, LOW);
        digitalWrite(backwardPinLeft, HIGH);
        digitalWrite(forwardPinLeft, LOW);
        server.send(200, "text/plain", "Moving backward.");
    });
    server.on("/releaseBackward", HTTP_GET, []() {
        server.send(200, "text/plain", "Backward stopped.");
    });

    // Right turn
    server.on("/right", HTTP_GET, []() {
        currentTurnDifference = defaultTurnDifference;
        analogWrite(rightSpeedPin, mainSpeed);
        analogWrite(leftSpeedPin, mainSpeed);
        Serial.println("Turning right...");
        digitalWrite(backwardPinRight, HIGH);
        digitalWrite(forwardPinRight, LOW);
        digitalWrite(backwardPinLeft, LOW);
        digitalWrite(forwardPinLeft, HIGH);
        server.send(200, "text/plain", "Turned right.");
    });
    server.on("/releaseRight", HTTP_GET, []() {
        server.send(200, "text/plain", "Right turn stopped.");
    });

    // Left turn
    server.on("/left", HTTP_GET, []() {
        currentTurnDifference = defaultTurnDifference;
        analogWrite(rightSpeedPin, mainSpeed);
        analogWrite(leftSpeedPin, mainSpeed);
        Serial.println("Turning left...");
        digitalWrite(backwardPinRight, LOW);
        digitalWrite(forwardPinRight, HIGH);
        digitalWrite(backwardPinLeft, HIGH);
        digitalWrite(forwardPinLeft, LOW);
        server.send(200, "text/plain", "Turned left.");
    });
    server.on("/releaseLeft", HTTP_GET, []() {
        server.send(200, "text/plain", "Left turn stopped.");
    });

    // Diagonal movements (Northwest, Northeast, Southwest, Southeast)
    server.on("/northwest", HTTP_GET, []() {
        if (getMotorDirection() == "forward") {
            int rightSpeed = mainSpeed;
            int leftSpeed = mainSpeed - currentTurnDifference;
          if(rightSpeed > 255){
              rightSpeed = 255 ;
            }
            if(rightSpeed < 0){
              rightSpeed = 0 ;
            }
            if(leftSpeed > 255){
              leftSpeed = 255;              
            }
            if(leftSpeed < 0 ){
                leftSpeed = 0;              
            }
           
            analogWrite(rightSpeedPin, rightSpeed);
            analogWrite(leftSpeedPin, leftSpeed);
            currentTurnDifference *= 2;
        }
        Serial.println("Moving northwest...");
        server.send(200, "text/plain", "Northwest.");
    });
    server.on("/releaseNorthwest", HTTP_GET, []() {
        server.send(200, "text/plain", "Northwest stopped.");
    });

    server.on("/northeast", HTTP_GET, []() {
        if (getMotorDirection() == "forward") {
            int rightSpeed = mainSpeed - currentTurnDifference;
            int leftSpeed = mainSpeed;
          if(rightSpeed > 255){
              rightSpeed = 255 ;
            }
            if(rightSpeed < 0){
              rightSpeed = 0 ;
            }
            if(leftSpeed > 255){
              leftSpeed = 255;              
            }
            if(leftSpeed < 0 ){
                leftSpeed = 0;              
            }
           
            analogWrite(rightSpeedPin, rightSpeed);
            analogWrite(leftSpeedPin, leftSpeed);
            currentTurnDifference *= 2;
        }
        Serial.println("Moving northeast...");
        server.send(200, "text/plain", "Northeast.");
    });
    server.on("/releaseNortheast", HTTP_GET, []() {
        server.send(200, "text/plain", "Northeast stopped.");
    });

    server.on("/southwest", HTTP_GET, []() {
        if (getMotorDirection() == "backward") {
            int rightSpeed = mainSpeed;
            int leftSpeed = mainSpeed - currentTurnDifference;
          if(rightSpeed > 255){
              rightSpeed = 255 ;
            }
            if(rightSpeed < 0){
              rightSpeed = 0 ;
            }
            if(leftSpeed > 255){
              leftSpeed = 255;              
            }
            if(leftSpeed < 0 ){
                leftSpeed = 0;              
            }
           
            analogWrite(rightSpeedPin, rightSpeed);
            analogWrite(leftSpeedPin, leftSpeed);
            currentTurnDifference *= 2;
        }
        Serial.println("Moving southwest...");
        server.send(200, "text/plain", "Southwest.");
    });
    server.on("/releaseSouthwest", HTTP_GET, []() {
        server.send(200, "text/plain", "Southwest stopped.");
    });

    server.on("/southeast", HTTP_GET, []() {
        if (getMotorDirection() == "backward") {
            int rightSpeed = mainSpeed - currentTurnDifference;
            int leftSpeed = mainSpeed;
            if(rightSpeed > 255){
              rightSpeed = 255 ;
            }
            if(rightSpeed < 0){
              rightSpeed = 0 ;
            }
            if(leftSpeed > 255){
              leftSpeed = 255;              
            }
            if(leftSpeed < 0 ){
                leftSpeed = 0;              
            }
           
            analogWrite(rightSpeedPin, rightSpeed);
            analogWrite(leftSpeedPin, leftSpeed);
            currentTurnDifference *= 2;
        }
        Serial.println("Moving southeast...");
        server.send(200, "text/plain", "Southeast.");
    });
    server.on("/releaseSoutheast", HTTP_GET, []() {
        server.send(200, "text/plain", "Southeast stopped.");
    });

    // Horn (buzzer)
    server.on("/horn", HTTP_GET, []() {
        Serial.println("Horn sounded...");
        tone(BUZZER_PIN,300,200);
        delay(400);
        tone(BUZZER_PIN,300,500);
        delay(1000);
        server.send(200, "text/plain", "Horn sounded.");
    });

    server.on("/releaseHorn", HTTP_GET, []() {
        server.send(200, "text/plain", "Horn stopped.");
    });
    server.on("/update",HTTP_GET,handleSpeedUpdate);
    server.onNotFound(handleNotFound);

    // Start the server
    server.begin();
}

void loop() {
    // Handle client requests
    server.handleClient();
}
