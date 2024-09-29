#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define BUZZER_PIN D0


int AnaHiz ;
int AnaDonmeFarkiDefault = 20 ; 
int AnaDonmeFarki = 20; 
int sagHizPin = D2; 
int ileriPinSag = D3; 
int geriPinSag = D4; 

int solHizPin = D7;
int ileriPinSol = D6;
int geriPinSol =D5;

int ledPin = D8;


const char* ssid = "MYSSID"; //BUNU DEĞİŞTİRMEYİ UNUTMA
const char* password = "123456789"; // BUNU DEĞİŞTİR

ESP8266WebServer server(80);

const char* htmlContent = R"=====(

<!DOCTYPE html>

<html lang="tr">
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
            padding: 40px; /* Butonları büyüttüm */
            font-size: 40px; /* Yazı boyutunu büyüttüm */
            border: 2px solid #007bff;
            border-radius: 5px;
            background-color: #007bff;
            color: #fff;
            cursor: pointer;
            transition: background-color 0.3s ease;
            margin: 5px;
            outline: none; /* Butonlara tıklanınca varsayılan vurguyu kaldırdım */
        }
        .button:active {
            transform: scale(0.9); /* Butona tıklandığında boyutunu küçülttüm */
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
    <h1 style="color:white;" >ARABA ADI</h1>
    <div class="container">
        <button id="turboButton"  onmousedown="basildi('turbo')" onmouseup="birakildi('turbo')" class="button" name='turbo' value='on'>Turbo</button>
        <div class="row">
            <button id="northwestButton" onmousedown="basildi('northwest')" onmouseup="birakildi('northwest')" class="button" name='direction' value='northwest'>↖</button>
            <button id="northButton"  onmousedown="basildi('forward')" onmouseup="birakildi('forward')" class="button" name='direction' value='forward'>↑</button>
            <button id="northeastButton" onmousedown="basildi('northeast')" onmouseup="birakildi('northeast')" class="button" name='direction' value='northeast'>↗</button>
        </div>
        <div class="row">
            <button id="leftButton" onmousedown="basildi('left')" onmouseup="birakildi('left')" class="button" name='direction' value='left'>←</button>
            <button id="stopButton" onmousedown="basildi('dur')" onmouseup="birakildi('dur')" class="button" name='direction' value='stop'>■</button>
            <button id="rightButton"  onmousedown="basildi('right')" onmouseup="birakildi('right')" onmousedown="basildi('right')" onmouseup="birakildiLeftOrRight()" class="button" name='direction' value='right'>→</button>
        </div>
        <div class="row">
            <button id="southwestButton"  onmousedown="basildi('southwest')" onmouseup="birakildi('southwest')"   class="button" name='direction' value='southwest'>↙</button>
            <button id="southButton" onmousedown="basildi('backward')" onmouseup="birakildi('backward')" class="button" name='direction' value='backward'>↓</button>
            <button id="southeastButton" onmousedown="basildi('southeast')" onmouseup="birakildi('southeast')"  class="button" name='direction' value='southeast'>↘</button>
        </div>
		<div id="output"></div>

        <input type='range' min='0' max='255' step='5' id="slider">
		<output for='slider' style="color:white;" id='volume'>0</output>

        <button id="hornButton"  onmousedown="basildi('duduk')" onmouseup="birakildi('duduk')" class="button" name='horn' value='on'>Düdük</button>
		
		
    </div>
	<script>
		
		var basildiMi = {};
		var slider = document.getElementById('slider');
		var volume = document.getElementById('volume');
		var xhr;

		slider.oninput = function() {
			
			var value = this.value ;
			volume.innerHTML = value;
			if(xhr && xhr.readyState != 4){
			    xhr.abort(); // Önceki isteği iptal et
			}
				xhr = new XMLHttpRequest();
				xhr.open('GET', '/update?value=' + value, true);
				xhr.send();
				
			
			
		}
		function basildi(yon){
			  if (!basildiMi[yon]) {
                basildiMi[yon] = true;
                motorKontrol(yon);
            }
		}
		
		
		function birakildi(yon){
            basildiMi[yon] = false;
			motorKontrolBirakildi(yon);
		
		}
		
		function motorKontrol(yon){
		  var xhr = new XMLHttpRequest();
		  var url =  yon ;
		  console.log(url);
		  xhr.open("GET", "http://192.168.4.1/" + url, true); // GENELDE 4 üzerinden IP ALIR AMA SİZDE FARKLI OLUR DENEYEREK YAPMAKTA FAYDA VAR
		  xhr.send();
			
		
		/*
			if(yon=='stop'){
			url = "/basildi/" + yon;
                    console.log(url);
				return;
			}
			var intervalID = setInterval(function() {
			
                if (basildiMi[yon]) {
                    url = "/basildi/" + yon;
                    console.log(url);
                } else {
                    clearInterval(intervalID);
                }
            }, 100);
			*/
		}
		function motorKontrolBirakildi(yon){

      if(yon == "turbo"){
        var xhr = new XMLHttpRequest();
        url = "birakildi"+ yon;
        console.log(url);
        xhr.open("GET", "http://192.168.4.1/" + url, true);
        xhr.send();


      }
     
		
		
		}
		
		
		

	</script>
</body>
</html>
)=====";


String ileriMiGeriMi(){
    int geriSagDurum = digitalRead(geriPinSag);
    int ileriSagDurum = digitalRead(ileriPinSag);
    int ileriSolDurum = digitalRead(ileriPinSol);
    int geriSolDurum = digitalRead(geriPinSol);

    if(geriSagDurum == 1 && geriSolDurum == 1 && ileriSagDurum ==0 && ileriSolDurum == 0){
      return "geri";
      
    }else if (geriSagDurum == 0 && geriSolDurum == 0 && ileriSagDurum == 1 && ileriSolDurum == 1) {
        return "ileri";
    } else if (geriSagDurum == 1 && geriSolDurum ==  0 && ileriSagDurum == 0 && ileriSolDurum == 1) {
        return "sag";
    } else if (geriSagDurum == 0 && geriSolDurum == 1 && ileriSagDurum == 1 && ileriSolDurum == 0) {
        return "sol";
    } else {
        return "dur";
    }


}
void handleRoot(){
  server.send(200,"text,html",htmlContent);
}
void handleNotFound(){
  server.send(404, "text/plain", "404 Not Found");
}
void handleUpdate() {
  if (server.argName(0) == "value") {
    String valueStr = server.arg(0);
    int value = valueStr.toInt(); 
    AnaHiz = value ; 
    analogWrite(sagHizPin,AnaHiz);
    analogWrite(solHizPin, AnaHiz);
    
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
    pinMode(ileriPinSag, OUTPUT);
    pinMode(geriPinSag, OUTPUT);
    pinMode(sagHizPin, OUTPUT);
    pinMode(ileriPinSol, OUTPUT);
    pinMode(geriPinSol, OUTPUT);
    pinMode(solHizPin, OUTPUT);


    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(ledPin,OUTPUT);


    Serial.begin(115200);


    // ESP8266'yı AP modunda başlat
    WiFi.softAP(ssid, password);
    Serial.println("AP Modu Başlatıldı");

    // IP adresini yazdır
    Serial.println("ESP8266 IP Adresi:");
    Serial.println(WiFi.softAPIP());

    // Sunucuyu başlat
    server.on("/", HTTP_GET,handleRoot);

  
    //turbo 
    server.on("/turbo", HTTP_GET, []() {
        digitalWrite(sagHizPin,HIGH);
        digitalWrite(solHizPin , HIGH);
        Serial.println("Turbo açılıyor...");
        server.send(200, "text/plain", "Turbo.");
    });
    server.on("/birakilditurbo", HTTP_GET, []() {
        analogWrite(sagHizPin,AnaHiz);
        analogWrite(solHizPin,AnaHiz);
        Serial.println("Turbo açılıyor...");
        server.send(200, "text/plain", "Turbo.");
    });


    //ileri
    server.on("/forward", HTTP_GET, []() {

        AnaDonmeFarki = AnaDonmeFarkiDefault ; 
        analogWrite(sagHizPin,AnaHiz);
        analogWrite(solHizPin, AnaHiz);
        
        Serial.println("İleri sürülüyor...");
        digitalWrite(geriPinSag, LOW);
        digitalWrite(ileriPinSag, HIGH); 
        digitalWrite(geriPinSol, LOW);
        digitalWrite(ileriPinSol, HIGH); 
        digitalWrite(ledPin,HIGH);
        server.send(200, "text/plain", "İleri sürüldü.");
    });
    server.on("/birakildiforward", HTTP_GET, []() {
        Serial.println("ileri bırakıldı...");

        server.send(200, "text/plain", "İleri bırakıldı.");
    });


    //geri
    server.on("/backward", HTTP_GET, []() {
        AnaDonmeFarki = AnaDonmeFarkiDefault ; 

        analogWrite(sagHizPin,AnaHiz);
        analogWrite(solHizPin, AnaHiz);
        Serial.println("Geri sürülüyor...");
        delay(500);
        digitalWrite(geriPinSag, HIGH);
        digitalWrite(ileriPinSag, LOW); 
        digitalWrite(geriPinSol, HIGH);
        digitalWrite(ileriPinSol, LOW); 


        server.send(200, "text/plain", "Geri sürüldü.");
    });
    server.on("/birakildibackward", HTTP_GET, []() {
        server.send(200, "text/plain", "birakildigeri.");
    });


    //right
    server.on("/right", HTTP_GET, []() {

        AnaDonmeFarki = AnaDonmeFarkiDefault ; 


        analogWrite(sagHizPin,AnaHiz);
        analogWrite(solHizPin, AnaHiz);
                
        Serial.println("Sağa dönülüyor...");

        digitalWrite(geriPinSag, HIGH);
        digitalWrite(ileriPinSag, LOW); 
        digitalWrite(geriPinSol, LOW);
        digitalWrite(ileriPinSol, HIGH); 


        
        
        server.send(200, "text/plain", "Sağa dönüldü.");
    });
    server.on("/birakildiright", HTTP_GET, []() {
        server.send(200, "text/plain", "sagbirak.");
    });



    //left
    server.on("/left", HTTP_GET, []() {
      AnaDonmeFarki = AnaDonmeFarkiDefault ; 

       analogWrite(sagHizPin,AnaHiz);
        analogWrite(solHizPin, AnaHiz);
        Serial.println("Sola dönülüyor...");
        digitalWrite(geriPinSag, LOW);
        digitalWrite(ileriPinSag, HIGH); 
        digitalWrite(geriPinSol, HIGH);
        digitalWrite(ileriPinSol, LOW); 
        server.send(200, "text/plain", "Sola dönüldü.");
    });

    server.on("/birakildileft", HTTP_GET, []() {
        server.send(200, "text/plain", "Sol birakildi.");
    });


    //sol ileri
    server.on("/northwest", HTTP_GET, []() {


    if (ileriMiGeriMi() == "ileri") {
      int sagHiz = AnaHiz ;
      int solHiz = AnaHiz- AnaDonmeFarki ;

      // Sağ ve sol hızları sınırlar içinde tut
      if (sagHiz > 255) {
        sagHiz = 255;
      }
      if (sagHiz < 0) {
        sagHiz = 0;
      }
      if (solHiz > 255) {
        solHiz = 255;
      }
      if (solHiz < 0) {
        solHiz = 0;
      }

      // Motor hızlarını ayarla
      analogWrite(sagHizPin, sagHiz);
      analogWrite(solHizPin, solHiz);

      AnaDonmeFarki = AnaDonmeFarki *2 ; 

    }
        Serial.println("sol ileri...");
        server.send(200, "text/plain", "norhwest.");
    });
    server.on("/birakildinorthwest", HTTP_GET, []() {
        server.send(200, "text/plain", "norhwest.");
    });


    //sağ ileri
    server.on("/northeast", HTTP_GET, []() {

    if (ileriMiGeriMi() == "ileri") {
      // İleri giderken sağa doğru hafif dönme
      int sagHiz = AnaHiz - AnaDonmeFarki;
      int solHiz = AnaHiz;

      // Sağ ve sol hızları sınırlar içinde tut
      if (sagHiz > 255) {
        sagHiz = 255;
      }
      if (sagHiz < 0) {
        sagHiz = 0;
      }
      if (solHiz > 255) {
        solHiz = 255;
      }
      if (solHiz < 0) {
        solHiz = 0;
      }

      // Motor hızlarını ayarla
      analogWrite(sagHizPin, sagHiz);
      analogWrite(solHizPin, solHiz);


      AnaDonmeFarki = AnaDonmeFarki * 2 ;



    }
        Serial.println("sağ ileri...");
        server.send(200, "text/plain", "northeast.");
    });
    server.on("/birakildinortheast", HTTP_GET, []() {
        server.send(200, "text/plain", "northeast.");
    });


    //sol geri
    server.on("/southwest", HTTP_GET, []() {
      
    if (ileriMiGeriMi() == "geri") {
      int sagHiz = AnaHiz ;
      int solHiz = AnaHiz - AnaDonmeFarki;

      // Sağ ve sol hızları sınırlar içinde tut
      if (sagHiz > 255) {
        sagHiz = 255;
      }
      if (sagHiz < 0) {
        sagHiz = 0;
      }
      if (solHiz > 255) {
        solHiz = 255;
      }
      if (solHiz < 0) {
        solHiz = 0;
      }
      // Motor hızlarını ayarla
      analogWrite(sagHizPin, sagHiz);
      analogWrite(solHizPin, solHiz);

      AnaDonmeFarki = AnaDonmeFarki * 2 ;

    }
        Serial.println("sol geri..");
        server.send(200, "text/plain", "southwest.");
    });
    server.on("/birakildisouthwest", HTTP_GET, []() {
        server.send(200, "text/plain", "southwest.");
    });


    //sag geri

    server.on("/southeast", HTTP_GET, []() {
      
    if (ileriMiGeriMi() == "geri") {
      int sagHiz = AnaHiz - AnaDonmeFarki;
      int solHiz = AnaHiz ;

      // Sağ ve sol hızları sınırlar içinde tut
      if (sagHiz > 255) {
        sagHiz = 255;
      }
      if (sagHiz < 0) {
        sagHiz = 0;
      }
      if (solHiz > 255) {
        solHiz = 255;
      }
      if (solHiz < 0) {
        solHiz = 0;
      }

      // Motor hızlarını ayarla
      analogWrite(sagHizPin, sagHiz);
      analogWrite(solHizPin, solHiz);

      AnaDonmeFarki = AnaDonmeFarki *2 ; 
    }
        Serial.println("sağ geri...");
        server.send(200, "text/plain", "southeast.");
    });


    server.on("/birakildisoutheast", HTTP_GET, []() {
      
        server.send(200, "text/plain", "southeast.");
    });



    //duduk

    server.on("/duduk", HTTP_GET, []() {
            Serial.println("duuut...");
            //tone(BUZZER_PIN,300,200);
            tone(BUZZER_PIN,300,200);
            delay(400);
            //tone(BUZZER_PIN,300,500);
            tone(BUZZER_PIN,300,500);
            delay(1000);
            server.send(200, "text/plain", "duuuuuuut.");
    });

    server.on("/birakildiduduk", HTTP_GET, []() {
           
            server.send(200, "text/plain", "biraköt.");
    });






    
    //dur
    server.on("/dur", HTTP_GET, []() {
        Serial.println("Motor durduruldu.");

        digitalWrite(geriPinSag, LOW);
        digitalWrite(ileriPinSag, LOW); 
        digitalWrite(geriPinSol, LOW);
        digitalWrite(ileriPinSol, LOW); 
        digitalWrite(ledPin,LOW);


        server.send(200, "text/plain", "Motor durduruldu.");
    });
    server.on("/birakildidur", HTTP_GET, []() {
        Serial.println("Motor durduruldu.");
        server.send(200, "text/plain", "Motor durduruldu.");
    });

    server.on("/update",HTTP_GET,handleUpdate);
    server.onNotFound(handleNotFound);


    server.begin();
}

void loop() {
    server.handleClient();
}
