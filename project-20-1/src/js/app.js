
var debtURL = "http://www.brillig.com/debt_clock";

// Called when the message send attempt succeeds
function messageSuccessHandler() {
  console.log("Message send succeeded.");  
}

// Called when the message send attempt fails
function messageFailureHandler() {
  console.log("Message send failed.");
}

function get_the_debt() {
   // Connect to the Web site
   var req = new XMLHttpRequest();  
   req.open('GET', debtURL, false);   
   req.send(null);  
   if(req.status != 200) return "0";
    
   var page = req.responseText;

   // Find the debt number
   var index = page.indexOf("ALT=\"$");
   var pos = page.indexOf("\"", index+6);
   var amount = page.substr(index+5, pos-index-5);
   var regexp = / /g;
   amount = amount.replace(regexp,"");

    return amount;
}

// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
  console.log("JS is ready!");
});
												
// Called when incoming message from the Pebble is received
// We are currently only checking the "message" appKey defined in appinfo.json/Settings
Pebble.addEventListener("appmessage", function(e) {
  var debt;
  var dictionary = e.payload;
  console.log('Got message: ' + JSON.stringify(dictionary));
  if (dictionary['ASK']) {
        debt = get_the_debt();
        console.log("Debt = " + debt);
        Pebble.sendAppMessage({"DEBT": debt}, messageSuccessHandler, messageFailureHandler);
  }
});