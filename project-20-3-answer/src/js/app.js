//-----------------------------------------------------------------------------
//
//  JavaScript side for getting latitude and longitude data.
//
//  Based VERY loosely on Speedometer for Pebble by Bernd Busse
//  https://github.com/tryone144/PebbleSpeedo/blob/master/src/location.js
//
//  Mike Jipping, August 2016

var watchID;
var latitude;
var longitude;
var altitude;
var accuracy;

// Called when the message send attempt succeeds
function messageSuccessHandler() {
  console.log("Message send succeeded.");  
}

// Called when the message send attempt fails
function messageFailureHandler() {
  console.log("Message send failed.");
}

// Geolocation Options for realtime applications
var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 1000, 
  timeout: 5000
};

// Geolocation Listener ID
// Success Callback
function locationSuccess(pos) {
    var location = pos.coords;

    // Convert values
    latitude = location.latitude ? Math.round(location.latitude * 100) | 0 : -1; // Latitude in Decimal Degrees (negative means south) -> Keep prec. 2
    longitude = location.longitude ? Math.round(location.longitude * 100) | 0 : -1; // Longitude in Decimal Degrees (negative means east) -> Keep prec. 2
    altitude = location.altitude ? Math.round(location.altitude) | 0 : -1; // Altitude in Meters above WGS84 Elipsoid
    accuracy = location.accuracy ? Math.round(location.accuracy) | 0 : -1; // Coordinates Accuracy in Meters
    
    Pebble.sendAppMessage({
         "LATITUDE": latitude,
         "LONGITUDE": longitude,
         "ALTITUDE": altitude,
         "ACCURACY": accuracy
    }, messageSuccessHandler, messageFailureHandler);
}

// Error Callback
function locationError(err) {
    console.log('ERROR: location error (' + err.code + '): ' + err.message);
}

// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
  console.log("JS is ready!");
});
												
// Called when incoming message from the Pebble is received
// We are currently only checking the "message" appKey defined in appinfo.json/Settings
Pebble.addEventListener("appmessage", function(e) {
    var dictionary = e.payload;
    
    if (dictionary['LATLONG']) {
        watchID = navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
    } else if (dictionary['ADDRESS']) {
        var load = dictionary['ADDRESS'];
        if (load[0] > 127) 
          latitude = (-load[0] ^ 0xff) + 1 - load[1]/100;
        else
          latitude = load[0] + load[1]/100;
        latitude = load[0] + load[1]/100;
        if (load[2] > 127) 
            longitude = (-load[2] ^ 0xff) + 1 - load[3]/100;
        else 
            longitude = load[2] + load[3]/100;
        
        // These URLs are restricted as to the number of accesses per minute
        // Get your own API key at mapzen.com
        var url = "http://pelias.mapzen.com/v1/reverse?api-key=valhalla-bb56Xzi&point.lat=" + latitude + "&point.lon="+longitude;
        var req = new XMLHttpRequest();  
        req.open('GET', url, false);   
        req.send(null);  
        if(req.status != 200) return "0";
      
        var text = req.responseText;
        var obj = JSON.parse(text);
        var len = obj.features.length;    
        var streetaddress = obj.features[len-1].properties.label;
      
        Pebble.sendAppMessage({"STREETADDRESS": streetaddress}, messageSuccessHandler, messageFailureHandler);
  }
});
